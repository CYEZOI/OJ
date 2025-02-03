#include <algorithm>
#include <cbor.h>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

std::vector<uint8_t> parseAuthenticatorData(std::vector<uint8_t> authData) {
    // 定义内部数据结构
    struct AuthenticatorFlags {
        bool up;
        bool uv;
        bool be;
        bool bs;
        bool at;
        bool ed;
        uint8_t flagsInt;
    };

    struct ParsedData {
        std::vector<uint8_t> rpIdHash;
        std::vector<uint8_t> flagsBuf;
        AuthenticatorFlags flags;
        uint32_t counter;
        std::vector<uint8_t> counterBuf;
        std::vector<uint8_t> aaguid;
        std::vector<uint8_t> credentialID;
        std::vector<uint8_t> credentialPublicKey;
        std::vector<uint8_t> extensionsDataBuffer;
    };

    // 参数检查
    if (authData.size() < 37) {
        throw std::runtime_error("Authenticator data too short");
    }

    ParsedData result;
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
    result.rpIdHash = {authData.begin(), authData.begin() + 32};
    pointer += 32;

    result.flagsBuf = {authData[pointer]};
    const uint8_t flagsInt = authData[pointer++];
    result.flags = {
        static_cast<bool>(flagsInt & 0x01), // UP
        static_cast<bool>(flagsInt & 0x04), // UV
        static_cast<bool>(flagsInt & 0x08), // BE
        static_cast<bool>(flagsInt & 0x10), // BS
        static_cast<bool>(flagsInt & 0x40), // AT
        static_cast<bool>(flagsInt & 0x80), // ED
        flagsInt};

    result.counter = getUint32BE(authData, pointer);
    result.counterBuf = {authData.begin() + pointer, authData.begin() + pointer + 4};
    pointer += 4;

    // 处理凭证数据
    if (result.flags.at) {
        result.aaguid = {authData.begin() + pointer, authData.begin() + pointer + 16};
        pointer += 16;

        const uint16_t credIDLen = getUint16BE(authData, pointer);
        pointer += 2;
        result.credentialID = {authData.begin() + pointer, authData.begin() + pointer + credIDLen};
        pointer += credIDLen;

        // Firefox 特殊处理
        constexpr uint8_t badCBOR[] = {0xa3, 0x01, 0x63, 0x4f, 0x4b, 0x50, 0x03, 0x27, 0x20, 0x67, 0x45, 0x64, 0x32, 0x35, 0x35, 0x31, 0x39};
        bool fixApplied = false;
        if (std::equal(std::begin(badCBOR), std::end(badCBOR), authData.begin() + pointer)) {
            authData[pointer] = 0xa4;
            fixApplied = true;
        }

        // CBOR 解析
        cbor_load_result load{};
        cbor_item_t* item = cbor_load(authData.data() + pointer, authData.size() - pointer, &load);
        if (!item || load.error.code != CBOR_ERR_NONE) {
            throw std::runtime_error("CBOR parse failed");
        }

        // 序列化处理
        unsigned char* serialized = nullptr;
        size_t serializedSize = cbor_serialize_alloc(item, &serialized, &serializedSize);
        result.credentialPublicKey = {serialized, serialized + serializedSize};
        free(serialized);
        cbor_decref(&item);

        if (fixApplied) {
            authData[pointer] = 0xa3;
        }
        pointer += load.read;
    }

    // 处理扩展数据
    if (result.flags.ed) {
        cbor_load_result load{};
        cbor_item_t* item = cbor_load(authData.data() + pointer, authData.size() - pointer, &load);
        if (!item || load.error.code != CBOR_ERR_NONE) {
            throw std::runtime_error("Extension data parse failed");
        }

        unsigned char* serialized = nullptr;
        size_t serializedSize = cbor_serialize_alloc(item, &serialized, &serializedSize);
        result.extensionsDataBuffer = {serialized, serialized + serializedSize};
        free(serialized);
        cbor_decref(&item);
        pointer += load.read;
    }

    // 最终验证
    if (pointer != authData.size()) {
        throw std::runtime_error("Data length mismatch");
    }

    // 组装最终结果（示例返回序列化数据）
    std::vector<uint8_t> output;
    auto append = [&](const auto& vec) { output.insert(output.end(), vec.begin(), vec.end()); };

    append(result.rpIdHash);
    append(result.flagsBuf);
    output.push_back(result.flags.flagsInt);
    append(result.counterBuf);
    append(result.aaguid);
    append(result.credentialID);
    append(result.credentialPublicKey);
    append(result.extensionsDataBuffer);

    return output;
}
