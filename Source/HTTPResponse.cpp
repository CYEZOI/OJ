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

#include "HTTPResponse.hpp"
#include "Logger.hpp"
#include "Settings.hpp"
#include <iomanip>
#include <sstream>

HTTP_RESPONSE::HTTP_RESPONSE() {
    std::stringstream Temp;
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    Temp << std::put_time(std::localtime(&now_time_t), "%a, %d %b %Y %H:%M:%S %z");
    Headers["Date"] = Temp.str();
}

void HTTP_RESPONSE::SetVersion(std::string Version) { this->Version = Version; }
void HTTP_RESPONSE::SetCode(unsigned short Code) {
    this->Code = Code;
    if (Messages[Code] == "") {
        this->Message = std::to_string(Code);
        Logger.Warning("Can not find message for HTTP response code " + this->Message);
    } else
        this->Message = Messages[Code];
}
void HTTP_RESPONSE::SetHeader(std::string Name, std::string Value) {
    if (Name == "Content-Type")
        Value += "; charset=utf-8";
    this->Headers[Name] = Value;
}
void HTTP_RESPONSE::SetBody(std::string Body) {
    this->Body = Body;
    this->Headers["Content-Length"] = std::to_string(this->Body.size());
}

std::string HTTP_RESPONSE::Stringify() {
    std::string ReturnValue = Version + " " + std::to_string(Code) + " " + Message + "\n";
    for (auto i : Headers)
        ReturnValue += i.first + ": " + i.second + "\n";
    ReturnValue += "\n";
    ReturnValue += Body;
    return ReturnValue;
}
