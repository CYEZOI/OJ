#include "Utilities.hpp"
#include "Logger.hpp"
#include "TestGroup.hpp"
#include "Submissions.hpp"
#include "TempTestData.hpp"
#include <string>
#include <thread>
#include <algorithm>
#include <dirent.h>

void TEST_GROUP::UpdateAllResults(JUDGE_RESULT Result)
{
    this->Result = Result;
    for (size_t i = 0; i < TestCases.size(); i++)
        TestCases[i].Result = Result;
}

RESULT TEST_GROUP::Judge()
{
    Result = JUDGE_RESULT::JUDGING;

    bool UpdateDatabaseSignal = true;
    std::thread UpdateDatabase(
        [this, &UpdateDatabaseSignal]()
        {
            while (UpdateDatabaseSignal)
            {
                TEMP_TEST_DATA::Update(*this);
                usleep(500'000);
            }
        });

    for (size_t i = 0; i < TestCases.size(); i++)
    {
        TEMP_TEST_DATA::Insert(TestCases[i]);
        if (fork() == 0)
        {
            OUTPUT_IF_FAILED(TestCases[i].Judge());
            exit(0);
        }
    }
    bool Judged = false;
    while (!Judged)
    {
        Judged = true;
        for (int i = 0; i < TestCases.size(); i++)
        {
            TEMP_TEST_DATA::Select(TestCases[i]);
            if (TestCases[i].Result >= JUDGE_RESULT::WAITING)
                Judged = false;
        }
        usleep(500'000);
    }
    for (size_t i = 0; i < TestCases.size(); i++)
    {
        TEMP_TEST_DATA::Select(TestCases[i]);
        TEMP_TEST_DATA::Delete(TestCases[i]);
        TestCases[i].TGID = TGID;
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

    Result = (SecondMaxCount == 0 || MaxResult != JUDGE_RESULT::ACCEPTED) ? MaxResult : SecondMaxResult;

    UpdateDatabaseSignal = false;
    UpdateDatabase.join();

    TEMP_TEST_DATA::Update(*this);

    CREATE_RESULT(true, "Test group " + std::to_string(TGID) + " of submission " + std::to_string(SID) + " judged")
}
