#include "HTTPResponse.hpp"
#include "Logger.hpp"
#include "Settings.hpp"
#include <iomanip>
#include <sstream>

HTTP_RESPONSE::HTTP_RESPONSE()
{

    std::stringstream Temp;
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    Temp << std::put_time(std::localtime(&now_time_t), "%a, %d %b %Y %H:%M:%S %z");
    Headers["Date"] = Temp.str();
}

void HTTP_RESPONSE::SetVersion(std::string Version) { this->Version = Version; }
void HTTP_RESPONSE::SetCode(unsigned short Code)
{
    this->Code = Code;
    if (Messages[Code] == "")
    {
        this->Message = std::to_string(Code);
        Logger.Warning("Can not find message for HTTP response code " + this->Message);
    }
    else
        this->Message = Messages[Code];
}
void HTTP_RESPONSE::SetHeader(std::string Name, std::string Value)
{
    if (Name == "Content-Type")
        Value += "; charset=utf-8";
    this->Headers[Name] = Value;
}
void HTTP_RESPONSE::SetBody(std::string Body)
{
    this->Body = Body;
    this->Headers["Content-Length"] = std::to_string(this->Body.size());
}

std::string HTTP_RESPONSE::Stringify()
{
    std::string ReturnValue = Version + " " + std::to_string(Code) + " " + Message + "\n";
    for (auto i : Headers)
        ReturnValue += i.first + ": " + i.second + "\n";
    ReturnValue += "\n";
    ReturnValue += Body;
    return ReturnValue;
}
