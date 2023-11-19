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

#pragma once

#include "Exception.hpp"

class FILES
{
public:
    static void CreateFileDownloadLink(int FID, std::string &FileDownloadLink);
    static void GetFileContent(std::string FileToken, std::string &FileContent, std::string &Filename, std::string &FileType);
    static void UploadFile(std::string Filename, std::string FileContent, std::string FileType, int UID, int &FID);
    static void DeleteFile(int FID);
};
