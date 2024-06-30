/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2024  langningchen

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

#include "Files.hpp"
#include "Database.hpp"
#include "Settings.hpp"
#include "Utilities.hpp"

void FILES::CreateFileDownloadLink(int FID, std::string &FileDownloadLink) {
    DATABASE::SELECT("Files")
        .Select("FileToken")
        .Where("FID", FID)
        .Execute(
            [&FileDownloadLink](auto Data) {
                if (Data.size() == 0)
                    throw EXCEPTION("File not found");
                FileDownloadLink = "/Files/" + Data[0]["FileToken"];
            });
}
void FILES::GetFileContent(std::string FileToken, std::string &FileContent, std::string &Filename, std::string &FileType) {
    DATABASE::SELECT("Files")
        .Select("Filename")
        .Select("FileType")
        .Where("FileToken", FileToken)
        .Execute(
            [FileToken, &FileContent, &Filename, &FileType](auto Data) {
                if (Data.size() == 0)
                    throw EXCEPTION("File not found");
                Filename = Data[0]["Filename"];
                FileType = Data[0]["FileType"];
                std::string JudgeUsername;
                SETTINGS::GetSettings("JudgeUsername", JudgeUsername);
                UTILITIES::LoadFile("/home/" + JudgeUsername + "/Files/" + FileToken, FileContent);
            });
}
void FILES::UploadFile(std::string Filename, std::string FileContent, std::string FileType, int UID, int &FID) {
    std::string FileToken = UTILITIES::RandomToken();
    std::string JudgeUsername;
    SETTINGS::GetSettings("JudgeUsername", JudgeUsername);
    UTILITIES::SaveFile("/home/" + JudgeUsername + "/Files/" + FileToken, FileContent);
    DATABASE::INSERT("Files")
        .Insert("Filename", Filename)
        .Insert("FileType", FileType)
        .Insert("FileToken", FileToken)
        .Insert("UID", UID)
        .Execute(
            [&FID](auto Data) {
                FID = Data;
            });
}
void FILES::DeleteFile(int FID) {
    std::string FileToken;
    DATABASE::SELECT("Files")
        .Select("FileToken")
        .Where("FID", FID)
        .Execute(
            [&FileToken](auto Data) {
                if (Data.size() == 0)
                    throw EXCEPTION("File not found");
                FileToken = Data[0]["FileToken"];
            });
    DATABASE::DELETE("Files")
        .Where("FID", FID)
        .Execute();
    std::string JudgeUsername;
    SETTINGS::GetSettings("JudgeUsername", JudgeUsername);
    if (unlink(("/home/" + JudgeUsername + "/Files/" + FileToken).c_str()) != 0)
        throw EXCEPTION("Failed to delete file");
}
