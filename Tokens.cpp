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

#include "Tokens.hpp"
#include "Database.hpp"
#include "Users.hpp"
#include "Utilities.hpp"

std::string TOKENS::CreateToken(int UID) {
    DATABASE::SELECT("Tokens")
        .Select("TokenValue")
        .Where("UID", UID)
        .Execute(
            [](auto Data) {
                if (Data.size() != 0)
                    DeleteToken(Data[0]["TokenValue"]);
            });
    std::string TokenValue = "";
    const std::string TokenCharList = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < 32; i++)
        TokenValue.push_back(TokenCharList[rand() % TokenCharList.size()]);
    DATABASE::INSERT("Tokens")
        .Insert("UID", UID)
        .Insert("TokenValue", TokenValue)
        .Execute();
    return TokenValue;
}
void TOKENS::CheckToken(std::string TokenValue) {
    DATABASE::SELECT("Tokens")
        .Select("CreateTime")
        .Where("TokenValue", TokenValue)
        .Execute(
            [TokenValue](auto Data) {
                if (Data.size() != 1)
                    throw EXCEPTION("Token invalid");
                if (UTILITIES::StringToTime(Data[0]["CreateTime"]) + 60 * 60 * 7 < time(NULL)) {
                    DeleteToken(TokenValue);
                    throw EXCEPTION("Token expired");
                }
            });
}
void TOKENS::DeleteToken(std::string TokenValue) {
    DATABASE::DELETE("Tokens")
        .Where("TokenValue", TokenValue)
        .Execute();
}
int TOKENS::GetUID(std::string TokenValue) {
    int UID;
    DATABASE::SELECT("Tokens")
        .Select("UID")
        .Where("TokenValue", TokenValue)
        .Execute(
            [&UID](auto Data) {
                if (Data.size() != 1)
                    throw EXCEPTION("Token invalid");
                UID = std::stoi(Data[0]["UID"]);
            });
    return UID;
}
