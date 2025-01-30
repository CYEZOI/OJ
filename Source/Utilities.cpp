/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2025  langningchen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**********************************************************************/

#include "Utilities.hpp"
#include "Settings.hpp"
#include <curl/curl.h>
#include <filesystem>
#include <openssl/sha.h>
#include <random>
#include <string.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <unistd.h>

std::string UTILITIES::RandomToken() {
    const std::string Characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string Token;
    std::mt19937 Generator(std::random_device());
    std::uniform_int_distribution<int> Distribution(0, Characters.length() - 1);
    for (int i = 0; i < 32; i++)
        Token += Characters[Distribution(Generator)];
    return Token;
}
std::string UTILITIES::StringReplaceAll(std::string Data, std::string Search, std::string Replace) {
    size_t Pos = Data.find(Search);
    while (Pos != std::string::npos) {
        Data.replace(Pos, Search.size(), Replace);
        Pos = Data.find(Search, Pos + Replace.size());
    }
    return Data;
}
std::vector<std::string> UTILITIES::StringSplit(std::string Data, std::string Delimiter) {
    std::vector<std::string> Result;
    size_t Pos = Data.find(Delimiter);
    while (Pos != std::string::npos) {
        Result.push_back(Data.substr(0, Pos));
        Data = Data.substr(Pos + Delimiter.size());
        Pos = Data.find(Delimiter);
    }
    Result.push_back(Data);
    return Result;
}
std::string UTILITIES::StringJoin(std::vector<std::string> Data, std::string Delimiter) {
    std::string Result = "";
    for (size_t i = 0; i < Data.size(); i++) {
        Result += Data[i];
        if (i != Data.size() - 1)
            Result += Delimiter;
    }
    return Result;
}
void UTILITIES::LoadFile(std::string Filename, std::string &Output) {
    FILE *File = fopen(Filename.c_str(), "rb");
    if (File == nullptr)
        throw EXCEPTION("Can not open file " + Filename);
    fseek(File, 0, SEEK_END);
    size_t Size = ftell(File);
    rewind(File);
    char *Buffer = new char[Size + 1];
    if (fread(Buffer, 1, Size, File) != Size) {
        fclose(File);
        delete[] Buffer;
        throw EXCEPTION("Can not read file " + Filename);
    }
    Buffer[Size] = '\0';
    Output.assign(Buffer, Size);
    delete[] Buffer;
    fclose(File);
}
void UTILITIES::LoadFile(std::string Filename, int &Output) {
    std::string Temp;
    LoadFile(Filename, Temp);
    Output = atoi(Temp.c_str());
}
void UTILITIES::SaveFile(std::string Filename, std::string Data) {
    FILE *File = fopen(Filename.c_str(), "wb");
    if (File == nullptr)
        throw EXCEPTION("Can not open file " + Filename);
    if (fwrite(Data.c_str(), 1, Data.size(), File) != Data.size()) {
        fclose(File);
        throw EXCEPTION("Can not write to file " + Filename);
    }
    fclose(File);
}
void UTILITIES::SaveFile(std::string Filename, int Data) {
    return SaveFile(Filename, std::to_string(Data));
}
std::string UTILITIES::RemoveSpaces(std::string Input) {
    while (Input.size() > 0 && (Input[0] == 0 ||
                                Input[0] == '\n' ||
                                Input[0] == '\r' ||
                                Input[0] == '\t' ||
                                Input[0] == ' '))
        Input.erase(0, 1);
    while (Input.size() > 0 && (Input[Input.size() - 1] == 0 ||
                                Input[Input.size() - 1] == '\n' ||
                                Input[Input.size() - 1] == '\r' ||
                                Input[Input.size() - 1] == '\t' ||
                                Input[Input.size() - 1] == ' '))
        Input.erase(Input.size() - 1, 1);
    return Input;
}
size_t UTILITIES::UploadFunction(char *ptr, size_t size, size_t nmemb, void *userp) {
    std::string *Upload = (std::string *)userp;
    const char *data = Upload->c_str();
    size_t room = size * nmemb;
    if (room < 1 || data == nullptr)
        return 0;
    size_t len = std::min(room, strlen(data));
    memcpy(ptr, data, len);
    Upload->erase(0, len);
    return len;
}
void UTILITIES::SendEmail(std::string To, std::string Subject, std::string Body) {
    std::string From = "";
    std::string Password = "";
    std::string Server = "";
    SETTINGS::GetSettings("EmailUsername", From);
    SETTINGS::GetSettings("EmailPassword", Password);
    SETTINGS::GetSettings("EmailServer", Server);
    From = UTILITIES::RemoveSpaces(From);
    Password = UTILITIES::RemoveSpaces(Password);
    Server = UTILITIES::RemoveSpaces(Server);
    CURL *Curl = curl_easy_init();
    if (Curl == nullptr)
        throw EXCEPTION("Can not initialize CURL");
    struct curl_slist *Recipients = nullptr;
    Recipients = curl_slist_append(Recipients, To.c_str());
    curl_easy_setopt(Curl, CURLOPT_URL, Server.c_str());
    curl_easy_setopt(Curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(Curl, CURLOPT_USERNAME, From.c_str());
    curl_easy_setopt(Curl, CURLOPT_PASSWORD, Password.c_str());
    curl_easy_setopt(Curl, CURLOPT_MAIL_FROM, From.c_str());
    curl_easy_setopt(Curl, CURLOPT_MAIL_RCPT, Recipients);
    curl_easy_setopt(Curl, CURLOPT_UPLOAD, true);
    std::string Message = "From: " + From + "\r\nTo: " + To + "\r\nSubject: " + Subject + "\r\n\r\n" + Body + "\r\n";
    curl_easy_setopt(Curl, CURLOPT_READDATA, &Message);
    curl_easy_setopt(Curl, CURLOPT_READFUNCTION, UploadFunction);
    CURLcode Result = curl_easy_perform(Curl);
    if (Result != CURLE_OK) {
        curl_slist_free_all(Recipients);
        curl_easy_cleanup(Curl);
        throw EXCEPTION("Can not send email: " + std::string(curl_easy_strerror(Result)));
    }
    curl_slist_free_all(Recipients);
    curl_easy_cleanup(Curl);
}
time_t UTILITIES::StringToTime(std::string String) {
    struct tm Time;
    memset(&Time, 0, sizeof(Time));
    if (sscanf(String.c_str(), "%d-%d-%d %d:%d:%d", &Time.tm_year, &Time.tm_mon, &Time.tm_mday, &Time.tm_hour, &Time.tm_min, &Time.tm_sec) != 6)
        return 0;
    Time.tm_year -= 1900;
    Time.tm_mon--;
    return mktime(&Time);
}
bool UTILITIES::VerifySignature(std::string Data, std::string Signature, std::string PublicKey) {
    // CredentialSignature, CredentialID, PublicKey
    Logger.Debug(HashData(Data));
    return true;
}
std::string UTILITIES::HashData(std::string Data) {
    unsigned char Buffer[SHA256_DIGEST_LENGTH];
    if (SHA256((const unsigned char *)Data.c_str(), Data.length(), Buffer) == NULL) {
        throw EXCEPTION("SHA256 failed");
    }
    std::stringstream StringStream;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        StringStream << std::hex << std::setw(2) << std::setfill('0') << (int)Buffer[i];
    }
    std::string Hash = StringStream.str();
    return Hash;
}
