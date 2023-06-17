#include "Result.hpp"

RESULT::RESULT()
{
    Success = true;
    Message = "No message";
}
RESULT::RESULT(bool Success, std::string Message, std::string DebugMessage)
{
    this->Success = Success;
    this->Message = Message;
    this->DebugMessage = DebugMessage;
    if (Success)
        Logger.Debug(DebugMessage);
    else
        Logger.Warning(DebugMessage);
}
