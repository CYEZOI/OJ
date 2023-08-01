#pragma once

#include "TestCase.hpp"
#include "Logger.hpp"
#include <vector>
#include <map>

class TEST_GROUP
{
private:
    int TGID;
    std::string PID;
    int SID;
    int Score = 0;
    JUDGE_RESULT Result = JUDGE_RESULT::WAITING;
    std::vector<TEST_CASE> TestCases;
    int TestCasesPassed = 0;
    std::string IOFilename;
    int ResultCount[JUDGE_RESULT::REJECTED] = {0};
    int Time = 0;
    int TimeSum = 0;
    int Memory = 0;

    void UpdateAllResults(JUDGE_RESULT Result);

    friend class API_PROCEED;
    friend class PROBLEM;
    friend class PROBLEMS;
    friend class SUBMISSION;
    friend class SUBMISSIONS;

public:
    RESULT Judge();
};
