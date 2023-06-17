#ifndef TEST_CASE_HPP
#define TEST_CASE_HPP

#include "Result.hpp"
#include "Settings.hpp"
#include "JudgeResult.hpp"
#include "Logger.hpp"
#include "Problem.hpp"

class TEST_CASE
{
private:
    int TCID = 0;
    pid_t ProcessID = 0;
    std::string Output;
    std::string StandardOutput;
    std::string StandardError;
    int SID = 0;
    std::string PID;
    int TGID = 0;
    std::string WorkDir;
    JUDGE_RESULT Result = JUDGE_RESULT::WAITING;
    std::string Description;
    int Time = 0;
    int Memory = 0;
    int Score = 0;
    int SystemCallCount[Settings.SystemCallCount] = {0};
    PROBLEM *Problem;
    TEST_CASE_DATA *UnjudgedTestCase;

    RESULT RedirectIO();
    RESULT SetupEnvrionment();
    RESULT RemoveEnvrionment();
    RESULT ChangeUser();
    RESULT SetLimits();
    RESULT ChildProcess();
    RESULT CheckSignal();
    RESULT CheckMemory();
    RESULT CheckSystemCall();
    RESULT ParentProcess();
    RESULT Run();
    RESULT Compare();

    friend class TEST_GROUP;
    friend class WEB_DATA_PROCEED;
    friend class PROBLEMS;
    friend class SUBMISSIONS;
    friend class SUBMISSION;

public:
    RESULT Judge();

    RESULT SetOutput(std::string Output);
    RESULT SetStandardOutput(std::string StandardOutput);
    RESULT SetStandardError(std::string StandardError);
    RESULT SetResult(JUDGE_RESULT Result);
    RESULT SetDescription(std::string Description);
    RESULT SetTime(int Time);
    RESULT SetMemory(int Memory);
    RESULT SetScore(int Score);
};

#endif
