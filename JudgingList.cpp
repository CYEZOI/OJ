#include "JudgingList.hpp"
#include "Submissions.hpp"
#include "Settings.hpp"
#include "Problems.hpp"
#include <thread>
#include <unistd.h>
#include <dirent.h>

void JUDGING_LIST::Init()
{
    new std::thread([this]()
                    {
        while (true)
        {
            while (JudgingList.empty())
                usleep(100000);
            SUBMISSION Submission;
            OUTPUT_IF_FAILED(SUBMISSIONS::GetSubmission(JudgingList.front(), Submission))
            JudgingList.pop();
            OUTPUT_IF_FAILED(Submission.Judge())
        } });
}

RESULT JUDGING_LIST::Add(SUBMISSION &Submission)
{
    RETURN_IF_FAILED(SUBMISSIONS::AddSubmission(Submission))
    JudgingList.push(Submission.SID);
    CREATE_RESULT(true, "Submission added to judging list")
}

JUDGING_LIST JudgingList;
