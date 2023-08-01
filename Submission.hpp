#pragma once

#include "TestGroup.hpp"
#include "Logger.hpp"
#include "Problem.hpp"
#include <vector>

class SUBMISSION
{
private:
    int SID = 0;
    int UID = 0;
    int ProcessID = 0;
    int Score = 0;
    int TestGroupsPassed = 0;
    std::string WorkDir;
    std::string Code;
    std::string PID;
    JUDGE_RESULT Result = JUDGE_RESULT::UNKNOWN_ERROR;
    std::string Description;
    std::vector<TEST_GROUP> TestGroups;
    int ResultCount[JUDGE_RESULT::REJECTED] = {0};
    int Time = 0;
    int TimeSum = 0;
    int Memory = 0;
    bool EnableO2 = false;
    time_t CreateTime;
    PROBLEM Problem;

    int JudgeUserID = 0;
    int JudgeUserGroupID = 0;
    std::string Compiler = "";
    std::string JudgeUsername = "";
    int TimeLimit = 1000;
    int OutputLimit = 1024 * 1024 * 1024;

    void UpdateAllResults(JUDGE_RESULT Result);
    RESULT RedirectIO();
    RESULT SetupEnvrionment();
    RESULT RemoveEnvrionment();
    RESULT ChangeUser();
    RESULT SetLimits();
    RESULT ChildProcess();
    RESULT ParentProcess();
    RESULT CreateFiles();
    RESULT Compile();
    RESULT RunTestGroups();

    friend class API_PROCEED;
    friend class JUDGING_LIST;
    friend class SUBMISSIONS;

public:
    RESULT Set(std::string Code, std::string PID);
    RESULT Judge();
};
