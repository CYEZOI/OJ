#include "Utilities.hpp"
#include "Logger.hpp"
#include "TestGroup.hpp"
#include "Submissions.hpp"
#include <string>
#include <thread>
#include <algorithm>
#include <dirent.h>

RESULT TEST_GROUP::UpdateAllResults(JUDGE_RESULT Result)
{
    RETURN_IF_FAILED(SetResult(Result));
    for (size_t i = 0; i < TestCases.size(); i++)
        RETURN_IF_FAILED(TestCases[i].SetResult(Result));
    CREATE_RESULT(true, "All results of test group " + std::to_string(TGID) + " of submission " + std::to_string(SID) + " updated")
}
RESULT TEST_GROUP::SetResult(JUDGE_RESULT Result)
{
    this->Result = Result;
    RETURN_IF_FAILED(SUBMISSIONS::UpdateTestGroup(this))
    CREATE_RESULT(true, "Result of test group " + std::to_string(TGID) + " of submission " + std::to_string(SID) + " updated")
}

RESULT TEST_GROUP::Judge()
{
    SetResult(JUDGE_RESULT::JUDGING);
    for (size_t i = 0; i < TestCases.size(); i++)
    {
        RETURN_IF_FAILED(TestCases[i].Judge());
        if (TestCases[i].Result == JUDGE_RESULT::ACCEPTED)
            TestCasesPassed++;
        Score += TestCases[i].Score;
        ResultCount[TestCases[i].Result]++;
        Time = std::max(Time, TestCases[i].Time);
        TimeSum += TestCases[i].Time;
        Memory = std::max(Memory, TestCases[i].Memory);
    }

    int MaxCount = 0;
    JUDGE_RESULT MaxResult = JUDGE_RESULT::UNKNOWN_ERROR;
    int SecondMaxCount = 0;
    JUDGE_RESULT SecondMaxResult = JUDGE_RESULT::UNKNOWN_ERROR;
    for (size_t i = 0; i < JUDGE_RESULT::REJECTED; i++)
        if (ResultCount[i] > MaxCount)
        {
            SecondMaxCount = MaxCount;
            SecondMaxResult = MaxResult;
            MaxCount = ResultCount[i];
            MaxResult = (JUDGE_RESULT)i;
        }
        else if (ResultCount[i] > SecondMaxCount)
        {
            SecondMaxCount = ResultCount[i];
            SecondMaxResult = (JUDGE_RESULT)i;
        }
    SetResult((SecondMaxCount == 0 || MaxResult != JUDGE_RESULT::ACCEPTED) ? MaxResult : SecondMaxResult);

    CREATE_RESULT(true, "Test group " + std::to_string(TGID) + " of submission " + std::to_string(SID) + " judged")
}
