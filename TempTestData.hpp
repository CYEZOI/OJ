#pragma once
#include "Submission.hpp"
class TEMP_TEST_DATA
{
public:
    static RESULT Insert(TEST_GROUP TestGroup);
    static RESULT Update(TEST_GROUP TestGroup);
    static RESULT Select(TEST_GROUP &TestGroup);
    static RESULT Delete(TEST_GROUP TestGroup);

    static RESULT Insert(TEST_CASE TestCase);
    static RESULT Update(TEST_CASE TestCase);
    static RESULT Select(TEST_CASE &TestCase);
    static RESULT Delete(TEST_CASE TestCase);
};
