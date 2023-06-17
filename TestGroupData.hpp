#ifndef TEST_GROUP_DATA_HPP
#define TEST_GROUP_DATA_HPP

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
    friend class WEB_DATA_PROCEED;

public:
    void AddTestCase(std::string Input, std::string Answer, int TimeLimit, int MemoryLimit, int Score);
};

#endif
