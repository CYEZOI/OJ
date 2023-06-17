#include "TestGroupData.hpp"

void TEST_GROUP_DATA::AddTestCase(std::string Input, std::string Answer, int TimeLimit, int MemoryLimit, int Score)
{
    TEST_CASE_DATA NewTestCase;
    NewTestCase.TCID = TestCases.size();
    NewTestCase.Input = Input;
    NewTestCase.Answer = Answer;
    NewTestCase.TimeLimit = TimeLimit;
    NewTestCase.MemoryLimit = MemoryLimit;
    NewTestCase.Score = Score;
    TestCases.push_back(NewTestCase);
}
