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

#define CREATE_JSON(Success, Message)             \
    {                                             \
        configor::json _ResponseJSON;             \
        _ResponseJSON["Success"] = (Success);     \
        _ResponseJSON["Message"] = (Message);     \
        _ResponseJSON["Data"] = configor::json(); \
        return _ResponseJSON;                     \
    }

#include <string>
#include "configor/json.hpp"
#include "Role.hpp"

class API_PROCEED
{
private:
    std::string Token;
    int UID;
    bool IsAdmin;
    std::string Action;
    configor::json Data;

    const configor::json BaseJSON = configor::json::parse("{\"Success\": false, \"Message\": \"\", \"Data\": {}}");
    static bool CheckTypes(configor::json JSON, std::vector<std::pair<std::string, configor::config_value_type>> Types);

    configor::json CheckTokenAvailable(std::string Token);

    configor::json CheckUsernameAvailable(std::string Username);
    configor::json CheckEmailAvailable(std::string EmailAddress);
    configor::json SendVerificationCode(std::string EmailAddress);
    configor::json Login(std::string Username, std::string Password);
    configor::json Register(std::string Username, std::string Nickname, std::string Password, std::string EmailAddress, std::string VerificationCode);
    configor::json ResetPassword(std::string EmailAddress, std::string VerificationCode, std::string Password);

    configor::json CreatePasskeyChallenge();
    configor::json DeletePasskeyChallenge(std::string Challenge);
    configor::json CreatePasskey(std::string Challenge, std::string CredentialID, std::string CredentialPublicKey);
    configor::json LoginWithPasskey(std::string Challenge, std::string CredentialID, int UserHandle, std::string CredentialSignature);

    configor::json AddUser(std::string Username, std::string Nickname, std::string Password, std::string EmailAddress, USER_ROLE Role);
    configor::json GetUser(int UID);
    configor::json UpdateUser(int UID, std::string Username, std::string Nickname, std::string HashedPassword, std::string EmailAddress, USER_ROLE Role);
    configor::json DeleteUser(int UID);
    configor::json GetUsers(int Page);
    configor::json HashPassword(std::string OriginalPassword);

    configor::json AddProblem(std::string PID, std::string Title, std::string IOFilename, std::string Description, std::string Input, std::string Output, std::string Range, std::string Hint, std::string Samples, std::string TestGroups);
    configor::json GetProblem(std::string PID);
    configor::json UpdateProblem(std::string PID, std::string Title, std::string IOFilename, std::string Description, std::string Input, std::string Output, std::string Range, std::string Hint, std::string Samples, std::string TestGroups);
    configor::json DeleteProblem(std::string PID);
    configor::json GetProblems(int Page);

    configor::json AddSubmission(std::string PID, bool EnableO2, std::string Code);
    configor::json GetSubmission(int SID);
    configor::json UpdateSubmission(int SID, std::string PID, int UID, std::string Code, int Result, std::string Description, int Time, int TimeSum, int Memory, int Score, bool EnableO2, std::string TestGroups);
    configor::json RejudgeSubmission(int SID);
    configor::json DeleteSubmission(int SID);
    configor::json GetSubmissions(int Page);

    configor::json GetSettings();
    configor::json SetSettings(configor::json SettingsJSON);

public:
    configor::json Proceed(configor::json Request);
};
