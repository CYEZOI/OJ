#ifndef SUBMISSIONS_HPP
#define SUBMISSIONS_HPP

#include "Database.hpp"
#include "Submission.hpp"

class SUBMISSIONS
{
private:
    static RESULT JSONToTestGroups(std::string JSONData, std::vector<TEST_GROUP> &TestGroups, std::string PID, int SID);
    static RESULT TestGroupsToJSON(std::vector<TEST_GROUP> TestGroups, std::string &JSONData);

public:
    static RESULT GetSubmission(int SID, SUBMISSION &Submission);
    static RESULT AddSubmission(SUBMISSION &Submission);
    static RESULT UpdateSubmission(SUBMISSION *Submission);
    static RESULT UpdateTestGroup(TEST_GROUP *TestGroup);
    static RESULT UpdateTestCase(TEST_CASE *TestCase);
};

#endif
