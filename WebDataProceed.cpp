#include "WebDataProceed.hpp"
#include "APIProceed.hpp"
#include "Result.hpp"
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
    std::string WebTheme = "";
    SETTINGS::GetSettings("WebTheme", WebTheme);
    std::string BasicFolder = "/home/langningc2009/OJ/HTML";
    HTTP_RESPONSE HTTPResponse;
    if (HTTPRequest.Path == "/api")
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
                    throw std::string("No token");
                int UID;
                THROW_STRING_ERROR_IF_FAILED(TOKENS::GetUID(UserToken, UID));
                bool IsAdmin;
                THROW_STRING_ERROR_IF_FAILED(USERS::IsAdmin(UID, IsAdmin));
                if (!IsAdmin)
                    throw std::string("Not admin");
                size_t BoundaryStartPosition = HTTPRequest.Headers["Content-Type"].find("boundary=");
                if (BoundaryStartPosition == std::string::npos)
                    throw std::string("No boundary");
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
                            throw std::string("No Content-Disposition");
                        if (Headers["Content-Type"] == "")
                            throw std::string("No Content-Type");
                        if (Body == "")
                            throw std::string("No Body");
                        size_t NameStartPosition = Headers["Content-Disposition"].find("filename=\"");
                        if (NameStartPosition == std::string::npos)
                            throw std::string("No filename start position");
                        NameStartPosition += 10;
                        size_t NameEndPosition = Headers["Content-Disposition"].find("\"", NameStartPosition);
                        if (NameEndPosition == std::string::npos)
                            throw std::string("No filename end position");
                        OriginalFilename = Headers["Content-Disposition"].substr(NameStartPosition, NameEndPosition - NameStartPosition);
                        std::string Filename = OriginalFilename;
                        std::string BadCharacters = "\\/:*?\"<>|";
                        for (int i = 0; i < BadCharacters.size(); i++)
                            Filename = UTILITIES::StringReplaceAll(Filename, BadCharacters.substr(i, 1), "");
                        if (Filename == "")
                            throw std::string("No filename");
                        else
                        {
                            int FID;
                            THROW_STRING_ERROR_IF_FAILED(FILES::UploadFile(Filename, Body, Headers["Content-Type"], UID, FID));
                            std::string FileDownloadLink;
                            THROW_STRING_ERROR_IF_FAILED(FILES::CreateFileDownloadLink(FID, FileDownloadLink));
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
            catch (std::string Exception)
            {
                ResponseJSON["code"] = 400;
                ResponseJSON["msg"] = Exception;
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
    else if (HTTPRequest.Path.length() == 39 && HTTPRequest.Path.substr(0, 7) == "/Files/")
    {
        std::string FileToken = HTTPRequest.Path.substr(7);
        std::string FileContent;
        std::string Filename;
        std::string FileType;
        if (!FILES::GetFileContent(FileToken, FileContent, Filename, FileType).Success)
            HTTPResponse.SetCode(404);
        else
        {
            HTTPResponse.SetBody(FileContent);
            HTTPResponse.SetHeader("Content-Disposition", "attachment; filename=\"" + Filename + "\"");
            HTTPResponse.SetHeader("Content-Type", FileType);
        }
    }
    else if (HTTPRequest.Path.length() >= 3 && HTTPRequest.Path.substr(HTTPRequest.Path.length() - 3, 3) == ".js")
    {
        std::string Data;
        if (!UTILITIES::LoadFile(BasicFolder + "/" + WebTheme + HTTPRequest.Path, Data).Success)
            HTTPResponse.SetCode(503);
        HTTPResponse.SetBody(Data);
        HTTPResponse.SetHeader("Content-Type", "application/javascript");
    }
    else if (HTTPRequest.Path.length() >= 4 && HTTPRequest.Path.substr(HTTPRequest.Path.length() - 4, 4) == ".css")
    {
        std::string Data;
        if (!UTILITIES::LoadFile(BasicFolder + "/" + WebTheme + HTTPRequest.Path, Data).Success)
            HTTPResponse.SetCode(503);
        HTTPResponse.SetBody(Data);
        HTTPResponse.SetHeader("Content-Type", "text/css");
    }
    else if (HTTPRequest.Path == "/TestAddProblem")
        API_PROCEED::TestAddProblem();
    else if (HTTPRequest.Path == "/Shutdown")
        exit(0);
    else
    {
        if (WebTheme == "BootStrap")
        {
            if (HTTPRequest.Path.length() >= 5 && HTTPRequest.Path.substr(HTTPRequest.Path.length() - 5, 5) == ".html")
            {
                std::string Data;
                if (!UTILITIES::LoadFile(BasicFolder + "/" + WebTheme + HTTPRequest.Path, Data).Success)
                    HTTPResponse.SetCode(404);
                HTTPResponse.SetBody(Data);
            }
            else if (HTTPRequest.Path == "/")
            {
                HTTPResponse.SetCode(302);
                HTTPResponse.SetHeader("location", "/index.html");
            }
            else
                HTTPResponse.SetCode(404);
        }
        else if (WebTheme == "WEUI")
        {
            std::string Data;
            if (!UTILITIES::LoadFile(BasicFolder + "/" + WebTheme + "/index.html", Data).Success)
                HTTPResponse.SetCode(500);
            HTTPResponse.SetBody(Data);
        }
        else
        {
            HTTPResponse.SetCode(503);
        }
    }
    return HTTPResponse;
}
