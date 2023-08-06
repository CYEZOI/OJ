#pragma once

#include <string>
#include "configor/json.hpp"
#include "Role.hpp"

class API_PROCEED
{
public:
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
