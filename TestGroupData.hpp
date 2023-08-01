#pragma once

#include <string>
#include <vector>
#include "TestCaseData.hpp"

class TEST_GROUP_DATA
{
private:
    int TGID = 0;
    std::vector<TEST_CASE_DATA> TestCases;

    friend class PROBLEMS;
    friend class SUBMISSIONS;
    friend class TEST_CASE;
    friend class API_PROCEED;

public:
    void AddTestCase(std::string Input, std::string Answer, int TimeLimit, int MemoryLimit, int Score);
};
