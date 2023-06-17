#include "Tokens.hpp"
#include "Database.hpp"
#include "Utilities.hpp"
#include "Users.hpp"

RESULT TOKENS::CreateToken(int UID, std::string &TokenValue)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Tokens")
                         .Select("TokenValue")
                         .Where("UID", UID)
                         .Execute(
                             [](auto Data)
                             {
                                 if (Data.size() != 0)
                                     RETURN_IF_FAILED(DeleteToken(Data[0]["TokenValue"]));
                                 CREATE_RESULT(true, "No exist token");
                             }))
    TokenValue = "";
    const std::string TokenCharList = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < 32; i++)
        TokenValue.push_back(TokenCharList[rand() % TokenCharList.size()]);
    RETURN_IF_FAILED(DATABASE::INSERT("Tokens")
                         .Insert("UID", UID)
                         .Insert("TokenValue", TokenValue)
                         .Execute())
    CREATE_RESULT(true, "Create token succeed");
}
RESULT TOKENS::CheckToken(std::string TokenValue)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Tokens")
                         .Select("CreateTime")
                         .Where("TokenValue", TokenValue)
                         .Execute(
                             [TokenValue](auto Data)
                             {
                                 CREATE_RESULT_IF_FALSE(Data.size() == 1, "Token invalid");
                                 CREATE_RESULT_IF_FALSE_WITH_OPERATION(UTILITIES::StringToTime(Data[0]["CreateTime"]) + 60 * 60 * 7 >= time(NULL), "Token expired", DeleteToken(TokenValue))
                                 CREATE_RESULT(true, "Token valid");
                             }))
    CREATE_RESULT(true, "Token valid");
}
RESULT TOKENS::DeleteToken(std::string TokenValue)
{
    RETURN_IF_FAILED(DATABASE::DELETE("Tokens")
                         .Where("TokenValue", TokenValue)
                         .Execute())
    CREATE_RESULT(true, "Delete token succeed");
}
