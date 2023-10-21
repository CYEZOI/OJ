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

#include "WebDataProceed.hpp"
#include "APIProceed.hpp"
#include "Excpetion.hpp"
#include "Utilities.hpp"
#include "Settings.hpp"
#include "Files.hpp"
#include "Tokens.hpp"
#include "Users.hpp"
#include <string>
#include <regex>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <list>
#include <algorithm>

HTTP_RESPONSE WEB_DATA_PROCEED::Proceed(HTTP_REQUEST HTTPRequest)
{
    std::string BasicFolder = "/home/langningc2009/OJ/HTML";
    std::string Initial = "";
    HTTP_RESPONSE HTTPResponse;
    std::string RequestPath = HTTPRequest.Path;
    size_t SearchParamsStartPosition = RequestPath.find("?");
    if (SearchParamsStartPosition != std::string::npos)
        RequestPath = RequestPath.substr(0, SearchParamsStartPosition);
    if (RequestPath == "/api")
    {
        HTTPResponse.SetHeader("Content-Type", "application/json");
        if (HTTPRequest.Verb != "POST")
        {
            HTTPResponse.SetCode(405);
            HTTPResponse.SetHeader("Allow", "POST");
        }
        else if (HTTPRequest.Headers["Content-Type"].find("multipart/form-data") != std::string::npos)
        {
            configor::json ResponseJSON;
            ResponseJSON["msg"] = "";
            ResponseJSON["code"] = 0;
            ResponseJSON["data"]["errFiles"] = configor::json::array({});
            try
            {
                std::string UserToken = HTTPRequest.Headers["X-Upload-Token"];
                if (UserToken == "")
                    throw EXCEPTION("No token");
                int UID;
                TOKENS::GetUID(UserToken, UID);
                bool IsAdmin;
                USERS::IsAdmin(UID, IsAdmin);
                if (!IsAdmin)
                    throw EXCEPTION("Not admin");
                size_t BoundaryStartPosition = HTTPRequest.Headers["Content-Type"].find("boundary=");
                if (BoundaryStartPosition == std::string::npos)
                    throw EXCEPTION("No boundary");
                std::string Boundary = HTTPRequest.Headers["Content-Type"].substr(BoundaryStartPosition + 9);
                std::vector<std::string> Data = UTILITIES::StringSplit(HTTPRequest.Body, "--" + Boundary);
                for (auto &i : Data)
                {
                    std::string OriginalFilename = "";
                    try
                    {
                        i = UTILITIES::RemoveSpaces(i);
                        if (i == "--" || i == "")
                            continue;
                        std::string Line;
                        std::map<std::string, std::string> Headers;
                        std::string Body;
                        for (size_t j = 0; j < i.length(); j++)
                        {
                            if (i[j] == '\n')
                            {
                                if (Line == "")
                                {
                                    Body = i.substr(j + 1);
                                    UTILITIES::SaveFile("/mnt/c/Data/data.wav", Body);
                                    break;
                                }
                                size_t NameStartPosition = 0;
                                size_t NameEndPosition = Line.find(": ", NameStartPosition);
                                std::string Name = Line.substr(NameStartPosition, NameEndPosition - NameStartPosition);

                                size_t ValueStartPosition = NameEndPosition + 2;
                                size_t ValueEndPosition = Line.size();
                                std::string Value = Line.substr(ValueStartPosition, ValueEndPosition - ValueStartPosition);

                                Headers[Name] = Value;

                                Line = "";
                            }
                            else
                                Line.push_back(i[j]);
                        }
                        if (Headers["Content-Disposition"] == "")
                            throw EXCEPTION("No Content-Disposition");
                        if (Headers["Content-Type"] == "")
                            throw EXCEPTION("No Content-Type");
                        if (Body == "")
                            throw EXCEPTION("No Body");
                        size_t NameStartPosition = Headers["Content-Disposition"].find("filename=\"");
                        if (NameStartPosition == std::string::npos)
                            throw EXCEPTION("No filename start position");
                        NameStartPosition += 10;
                        size_t NameEndPosition = Headers["Content-Disposition"].find("\"", NameStartPosition);
                        if (NameEndPosition == std::string::npos)
                            throw EXCEPTION("No filename end position");
                        OriginalFilename = Headers["Content-Disposition"].substr(NameStartPosition, NameEndPosition - NameStartPosition);
                        std::string Filename = OriginalFilename;
                        std::string BadCharacters = "\\/:*?\"<>|";
                        for (size_t i = 0; i < BadCharacters.size(); i++)
                            Filename = UTILITIES::StringReplaceAll(Filename, BadCharacters.substr(i, 1), "");
                        if (Filename == "")
                            throw EXCEPTION("No filename");
                        else
                        {
                            int FID;
                            FILES::UploadFile(Filename, Body, Headers["Content-Type"], UID, FID);
                            std::string FileDownloadLink;
                            FILES::CreateFileDownloadLink(FID, FileDownloadLink);
                            ResponseJSON["data"]["succMap"][OriginalFilename] = FileDownloadLink;
                        }
                    }
                    catch (std::string Exception)
                    {
                        ResponseJSON["data"]["errFiles"].push_back(OriginalFilename);
                        ResponseJSON["msg"] = Exception;
                    }
                }
            }
            catch (EXCEPTION ErrorData)
            {
                ResponseJSON["code"] = 400;
                ResponseJSON["msg"] = ErrorData.Message;
            }
            HTTPResponse.SetBody(ResponseJSON.dump());
        }
        else
        {
            try
            {
                configor::json RequestJSON = configor::json::parse(HTTPRequest.Body);
                API_PROCEED APIProceed;
                configor::json ResponseJSON = APIProceed.Proceed(RequestJSON);
                HTTPResponse.SetBody(ResponseJSON.dump());
            }
            catch (const configor::configor_exception &e)
            {
                HTTPResponse.SetCode(400);
                HTTPResponse.SetBody(e.what());
            }
        }
    }
    else if (RequestPath.length() == 39 && RequestPath.substr(0, 7) == "/Files/")
    {
        std::string FileToken = RequestPath.substr(7);
        std::string FileContent;
        std::string Filename;
        std::string FileType;
        try
        {
            FILES::GetFileContent(FileToken, FileContent, Filename, FileType);
            HTTPResponse.SetBody(FileContent);
            HTTPResponse.SetHeader("Content-Disposition", "attachment; filename=\"" + Filename + "\"");
            HTTPResponse.SetHeader("Content-Type", FileType);
        }
        catch (EXCEPTION ErrorData)
        {
            HTTPResponse.SetCode(404);
        }
    }
    else if (RequestPath == "/Shutdown")
        exit(0);
    else if (RequestPath == "/")
    {
        HTTPResponse.SetCode(302);
        HTTPResponse.SetHeader("location", "/index.html");
    }
    else
    {
        std::string Data;
        try
        {
            UTILITIES::LoadFile(BasicFolder + RequestPath, Data);
        }
        catch (EXCEPTION ErrorData)
        {
            HTTPResponse.SetCode(404);
        }
        HTTPResponse.SetBody(Data);
        if (RequestPath.length() >= 3 && RequestPath.substr(RequestPath.length() - 3, 3) == ".js")
            HTTPResponse.SetHeader("Content-Type", "application/javascript");
        else if (RequestPath.length() >= 4 && RequestPath.substr(RequestPath.length() - 4, 4) == ".css")
            HTTPResponse.SetHeader("Content-Type", "text/css");
        else if (RequestPath.length() >= 5 && RequestPath.substr(RequestPath.length() - 5, 5) == ".html")
            HTTPResponse.SetHeader("Content-Type", "text/html");
    }
    return HTTPResponse;
}
