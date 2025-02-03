#include "Passkey.hpp"
#include "Database.hpp"
#include "Utilities.hpp"
#include <cbor.h>
#include <nlohmann/json.hpp>

PASSKEY::AuthenticatorFlags::AuthenticatorFlags(uint8_t flagsInt) : flagsInt(flagsInt) {
    up = static_cast<bool>(flagsInt & 0x01);
    uv = static_cast<bool>(flagsInt & 0x04);
    be = static_cast<bool>(flagsInt & 0x08);
    bs = static_cast<bool>(flagsInt & 0x10);
    at = static_cast<bool>(flagsInt & 0x40);
    ed = static_cast<bool>(flagsInt & 0x80);
}
std::string PASSKEY::AuthenticatorFlags::toString() const {
    std::stringstream ss;
    ss << "UP: " << up << ", UV: " << uv
       << ", BE: " << be << ", BS: " << bs
       << ", AT: " << at << ", ED: " << ed
       << " (0x" << std::hex << static_cast<int>(flagsInt) << ")";
    return ss.str();
}

void PASSKEY::ParsedData::logDebugInfo() const {
    auto bytesToHex = [](const std::vector<uint8_t>& bytes) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (uint8_t b : bytes) {
            ss << std::setw(2) << static_cast<int>(b);
        }
        return ss.str();
    };

    Logger.Warning("[AuthData] RPID Hash: " + rpIdHash);
    Logger.Warning("[AuthData] Flags: " + flags.toString());
    Logger.Warning("[AuthData] Counter: " + std::to_string(counter));

    if (!aaguid.empty()) {
        Logger.Warning("[AuthData] AAGUID: " + bytesToHex(aaguid));
    }
    if (!credentialID.empty()) {
        Logger.Warning("[AuthData] CredentialID (" + std::to_string(credentialID.size()) + " bytes): " + bytesToHex(credentialID));
    }
    if (!credentialPublicKey.empty()) {
        Logger.Warning("[AuthData] PublicKey (" + std::to_string(credentialPublicKey.size()) + " bytes)");
    }
    if (!extensionsDataBuffer.empty()) {
        Logger.Warning("[AuthData] Extensions (" + std::to_string(extensionsDataBuffer.size()) + " bytes)");
    }
}

PASSKEY::ParsedData::ParsedData(std::vector<uint8_t> AuthData) {
    if (AuthData.size() < 37)
        throw EXCEPTION("Invalid auth data");

    size_t pointer = 0;

    // 自动内存管理工具
    auto getUint16BE = [](const std::vector<uint8_t>& data, size_t offset) {
        return (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
    };

    auto getUint32BE = [](const std::vector<uint8_t>& data, size_t offset) {
        return (static_cast<uint32_t>(data[offset]) << 24) |
               (static_cast<uint32_t>(data[offset + 1]) << 16) |
               (static_cast<uint32_t>(data[offset + 2]) << 8) |
               data[offset + 3];
    };

    // 解析基础数据
    std::stringstream ss;
    for (int i = 0; i < 32; i++) ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(AuthData[i]);
    rpIdHash = ss.str();
    pointer += 32;

    flagsBuf = {AuthData[pointer]};
    const uint8_t flagsInt = AuthData[pointer++];
    flags = AuthenticatorFlags(flagsInt);

    counter = getUint32BE(AuthData, pointer);
    counterBuf = {AuthData.begin() + pointer, AuthData.begin() + pointer + 4};
    pointer += 4;

    // 处理凭证数据
    if (flags.at) {
        aaguid = {AuthData.begin() + pointer, AuthData.begin() + pointer + 16};
        pointer += 16;

        const uint16_t credIDLen = getUint16BE(AuthData, pointer);
        pointer += 2;
        credentialID = {AuthData.begin() + pointer, AuthData.begin() + pointer + credIDLen};
        pointer += credIDLen;
        // CBOR 解析
        cbor_load_result load{};
        cbor_item_t* item = cbor_load(AuthData.data() + pointer, AuthData.size() - pointer, &load);
        if (!item || load.error.code != CBOR_ERR_NONE) {
            throw std::runtime_error("CBOR parse failed");
        }

        // 序列化处理
        unsigned char* serialized = nullptr;
        size_t serializedSize = cbor_serialize_alloc(item, &serialized, &serializedSize);
        credentialPublicKey = {serialized, serialized + serializedSize};
        free(serialized);
        cbor_decref(&item);

        pointer += load.read;
    }

    // 处理扩展数据
    if (flags.ed) {
        cbor_load_result load{};
        cbor_item_t* item = cbor_load(AuthData.data() + pointer, AuthData.size() - pointer, &load);
        if (!item || load.error.code != CBOR_ERR_NONE) {
            throw std::runtime_error("Extension data parse failed");
        }

        unsigned char* serialized = nullptr;
        size_t serializedSize = cbor_serialize_alloc(item, &serialized, &serializedSize);
        extensionsDataBuffer = {serialized, serialized + serializedSize};
        free(serialized);
        cbor_decref(&item);
        pointer += load.read;
    }

    // 最终验证
    if (pointer != AuthData.size()) {
        throw std::runtime_error("Data length mismatch");
    }
}

std::string PASSKEY::CreateChallenge() {
    std::string Challenge = UTILITIES::RandomToken();
    DATABASE::INSERT("PasskeyChallenges")
        .Insert("Challenge", Challenge)
        .Execute();
    return Challenge;
}
void PASSKEY::DeleteChallenge(std::string Challenge) {
    DATABASE::DELETE("PasskeyChallenges")
        .Where("Challenge", Challenge)
        .Execute();
}
void PASSKEY::CreatePasskey(int UID, configor::json Credential) {
    if (Credential["id"].as_string() != Credential["rawId"].as_string())
        throw EXCEPTION("Invalid credential");
    if (Credential["type"].as_string() != "public-key")
        throw EXCEPTION("Unexpected credential type");
    configor::json ClientDataJSON = configor::json::parse(UTILITIES::Base64Decode(Credential["response"]["clientDataJSON"].as_string()));
    if (ClientDataJSON["type"].as_string() != "webauthn.create")
        throw EXCEPTION("Unexpected client data type");
    if (ClientDataJSON["origin"].as_string() != "http://localhost")
        throw EXCEPTION("Invalid origin");

    nlohmann::json AttestationObjectJSON = nlohmann::json::from_cbor(UTILITIES::Base64Decode(Credential["response"]["attestationObject"].as_string()));
    Logger.Warning("AttestationObjectJSON: " + AttestationObjectJSON.dump());

    std::vector<uint8_t> AuthData;
    for (auto item : AttestationObjectJSON["authData"].get_binary()) AuthData.push_back(item);
    ParsedData ParsedData(AuthData);
    ParsedData.logDebugInfo();
    if (ParsedData.rpIdHash != UTILITIES::SHA256("localhost")) throw EXCEPTION("Invalid RP ID hash");
    if (!ParsedData.flags.up) throw EXCEPTION("User presence required");
    if (!ParsedData.flags.uv) throw EXCEPTION("User verification required");
    if (ParsedData.credentialID.empty()) throw EXCEPTION("Credential ID required");
    if (ParsedData.credentialPublicKey.empty()) throw EXCEPTION("Credential public key required");
    if (ParsedData.aaguid.empty()) throw EXCEPTION("AAGUID required");

    // DATABASE::SELECT("PasskeyChallenges")
    //     .Select("CreateTime")
    //     .Where("Challenge", Challenge)
    //     .Execute(
    //         [Challenge](auto Data) {
    //             if (Data.size() != 1)
    //                 throw EXCEPTION("Invalid challenge");
    //             DATABASE::DELETE("PasskeyChallenges")
    //                 .Where("Challenge", Challenge)
    //                 .Execute();
    //             if (UTILITIES::StringToTime(Data[0]["CreateTime"]) + 60 < time(NULL))
    //                 throw EXCEPTION("Challenge expired");
    //         });
    // DATABASE::INSERT("Passkeys")
    //     .Insert("UID", UID)
    //     .Insert("Credential", Credential)
    //     .Insert("PublicKey", PublicKey)
    //     .Execute();
}
std::string PASSKEY::GetPasskey(int UID, std::string Credential) {
    std::string Passkey;
    DATABASE::SELECT("Passkeys")
        .Select("PublicKey")
        .Where("UID", UID)
        .Where("Credential", Credential)
        .Execute(
            [&Passkey](auto Data) {
                if (Data.size() != 1)
                    throw EXCEPTION("Invalid credential");
                Passkey = Data[0]["PublicKey"];
            });
    return Passkey;
}
