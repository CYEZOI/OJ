#pragma once

#include "Result.hpp"

class FILES
{
private:
    static std::string RandomToken();

public:
    static RESULT CreateFileDownloadLink(int FID, std::string &FileDownloadLink);
    static RESULT GetFileContent(std::string FileToken, std::string &FileContent, std::string &Filename, std::string &FileType);
    static RESULT UploadFile(std::string Filename, std::string FileContent, std::string FileType, int UID, int &FID);
    static RESULT DeleteFile(int FID);
};
