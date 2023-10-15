/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2023  langningchen

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
#include "Utilities.hpp"
#include "Settings.hpp"

std::string FILES::RandomToken()
{
    const std::string Characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string Token;
    for (int i = 0; i < 32; i++)
        Token += Characters[rand() % Characters.length()];
    return Token;
}

RESULT FILES::CreateFileDownloadLink(int FID, std::string &FileDownloadLink)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Files")
                         .Select("FileToken")
                         .Where("FID", FID)
                         .Execute(
                             [&FileDownloadLink](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "File not found");
                                 FileDownloadLink = "/Files/" + Data[0]["FileToken"];
                                 CREATE_RESULT(true, "File download link created successfully");
                             }));
    CREATE_RESULT(true, "File download link created successfully");
}
RESULT FILES::GetFileContent(std::string FileToken, std::string &FileContent, std::string &Filename, std::string &FileType)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Files")
                         .Select("Filename")
                         .Select("FileType")
                         .Where("FileToken", FileToken)
                         .Execute(
                             [FileToken, &FileContent, &Filename, &FileType](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "File not found");
                                 Filename = Data[0]["Filename"];
                                 FileType = Data[0]["FileType"];
                                 std::string JudgeUsername;
                                 RETURN_IF_FAILED(SETTINGS::GetSettings("JudgeUsername", JudgeUsername));
                                 RETURN_IF_FAILED(UTILITIES::LoadFile("/home/" + JudgeUsername + "/Files/" + FileToken, FileContent))
                                 CREATE_RESULT(true, "File content retrieved successfully");
                             }));
    CREATE_RESULT(true, "File content retrieved successfully");
}
RESULT FILES::UploadFile(std::string Filename, std::string FileContent, std::string FileType, int UID, int &FID)
{
    std::string FileToken = RandomToken();
    std::string JudgeUsername;
    RETURN_IF_FAILED(SETTINGS::GetSettings("JudgeUsername", JudgeUsername));
    RETURN_IF_FAILED(UTILITIES::SaveFile("/home/" + JudgeUsername + "/Files/" + FileToken, FileContent));
    RETURN_IF_FAILED_WITH_OPERATION(DATABASE::INSERT("Files")
                                        .Insert("Filename", Filename)
                                        .Insert("FileType", FileType)
                                        .Insert("FileToken", FileToken)
                                        .Insert("UID", UID)
                                        .Execute(
                                            [&FID](auto Data)
                                            {
                                                FID = Data;
                                                CREATE_RESULT(true, "File uploaded successfully");
                                            }),
                                    unlink(("/home/" + JudgeUsername + "/Files/" + FileToken).c_str()));
    CREATE_RESULT(true, "File uploaded successfully");
}
RESULT FILES::DeleteFile(int FID)
{
    std::string FileToken;
    RETURN_IF_FAILED(DATABASE::SELECT("Files")
                         .Select("FileToken")
                         .Where("FID", FID)
                         .Execute(
                             [&FileToken](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "File not found");
                                 FileToken = Data[0]["FileToken"];
                                 CREATE_RESULT(true, "File token retrieved successfully");
                             }));
    RETURN_IF_FAILED(DATABASE::DELETE("Files")
                         .Where("FID", FID)
                         .Execute());
    std::string JudgeUsername;
    RETURN_IF_FAILED(SETTINGS::GetSettings("JudgeUsername", JudgeUsername));
    RETURN_IF_FALSE(unlink(("/home/" + JudgeUsername + "/Files/" + FileToken).c_str()) == 0, "Failed to delete file");
    CREATE_RESULT(true, "File deleted successfully");
}
