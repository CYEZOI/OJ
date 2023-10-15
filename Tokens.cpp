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
    CREATE_RESULT(true, "Create token succeeds");
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
    CREATE_RESULT(true, "Delete token succeeds");
}
RESULT TOKENS::GetUID(std::string TokenValue, int &UID)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Tokens")
                         .Select("UID")
                         .Where("TokenValue", TokenValue)
                         .Execute(
                             [&UID](auto Data)
                             {
                                 CREATE_RESULT_IF_FALSE(Data.size() == 1, "Token invalid");
                                 UID = std::stoi(Data[0]["UID"]);
                                 CREATE_RESULT(true, "Get UID succeeds");
                             }))
    CREATE_RESULT(true, "Get UID succeeds");
}
