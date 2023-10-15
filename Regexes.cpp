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

#include "Regexes.hpp"

RESULT REGEXES::CheckUsername(std::string Username)
{
    if (std::regex_match(Username, std::regex("^[0-9a-zA-Z]{4,16}$")))
        CREATE_RESULT(true, "Username valid")
    CREATE_RESULT(false, "Username invalid")
}
RESULT REGEXES::CheckPassword(std::string Password)
{
    if (!std::regex_match(Password, std::regex("^([^a-z]+|[^A-Z]+|[^0-9]+|[a-zA-Z0-9]+|)$")))
        CREATE_RESULT(true, "Password valid")
    CREATE_RESULT(false, "Password invalid")
}
RESULT REGEXES::CheckNickname(std::string Nickname)
{
    if (std::regex_match(Nickname, std::regex("^.{4,16}$")))
        CREATE_RESULT(true, "Nickname valid")
    CREATE_RESULT(false, "Nickname invalid")
}
RESULT REGEXES::CheckEmailAddress(std::string EmailAddress)
{
    if (std::regex_match(EmailAddress, std::regex("^([a-zA-Z0-9_-])+@([a-zA-Z0-9_-])+(.[a-zA-Z0-9_-])+$")))
        CREATE_RESULT(true, "Email address valid")
    CREATE_RESULT(false, "Email address invalid")
}
RESULT REGEXES::CheckVerificationCode(std::string VerificationCode)
{
    if (std::regex_match(VerificationCode, std::regex("^[0-9]{6}$")))
        CREATE_RESULT(true, "Verification code valid")
    CREATE_RESULT(false, "Verification code invalid")
}
