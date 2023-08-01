#pragma once

#include "Database.hpp"
#include "Submission.hpp"

class SUBMISSIONS
{
private:
    static RESULT JSONToTestGroups(std::string JSONData, std::vector<TEST_GROUP> &TestGroups, std::string PID, int SID);
    static RESULT TestGroupsToJSON(std::vector<TEST_GROUP> TestGroups, std::string &JSONData);

    friend class API_PROCEED;

public:
    static RESULT
    AddSubmission(SUBMISSION &Submission);
    static RESULT GetSubmission(int SID, SUBMISSION &Submission);
    static RESULT UpdateSubmission(SUBMISSION Submission);
    static RESULT UpdateSubmission(SUBMISSION *Submission);
    static RESULT DeleteSubmission(int SID);
};
