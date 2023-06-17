#include "Submissions.hpp"
#include "Utilities.hpp"
#include "Problems.hpp"
#include "configor/json.hpp"

RESULT SUBMISSIONS::JSONToTestGroups(std::string JSONData, std::vector<TEST_GROUP> &TestGroups, std::string PID, int SID)
{
    try
    {
        configor::json JSON = configor::json::parse(JSONData);
        for (auto TestGroup : JSON)
        {
            int TGID = TestGroup["TGID"].as_integer();
            TestGroups[TGID].Result = (JUDGE_RESULT)TestGroup["Result"].as_integer();
            TestGroups[TGID].Time = TestGroup["Time"].as_integer();
            TestGroups[TGID].TimeSum = TestGroup["TimeSum"].as_integer();
            TestGroups[TGID].Memory = TestGroup["Memory"].as_integer();
            for (auto TestCase : TestGroup["TestCases"])
            {
                int TCID = TestCase["TCID"].as_integer();
                TestGroups[TGID].TestCases[TCID].Output = TestCase["Output"].as_string();
                TestGroups[TGID].TestCases[TCID].StandardOutput = TestCase["StandardOutput"].as_string();
                TestGroups[TGID].TestCases[TCID].StandardError = TestCase["StandardError"].as_string();
                TestGroups[TGID].TestCases[TCID].Result = (JUDGE_RESULT)TestCase["Result"].as_integer();
                TestGroups[TGID].TestCases[TCID].Description = TestCase["Description"].as_string();
                TestGroups[TGID].TestCases[TCID].Time = TestCase["Time"].as_integer();
                TestGroups[TGID].TestCases[TCID].Memory = TestCase["Memory"].as_integer();
                TestGroups[TGID].TestCases[TCID].Score = TestCase["Score"].as_integer();
            }
        }
    }
    catch (std::exception &e)
    {
        CREATE_RESULT(false, e.what());
    }
    CREATE_RESULT(true, "Success");
}
RESULT SUBMISSIONS::TestGroupsToJSON(std::vector<TEST_GROUP> TestGroups, std::string &JSONData)
{
    try
    {
        configor::json JSON = configor::json::array({});
        for (auto &TestGroup : TestGroups)
        {
            configor::json NewTestGroup;
            NewTestGroup["TGID"] = TestGroup.TGID;
            NewTestGroup["Result"] = (int)TestGroup.Result;
            NewTestGroup["Time"] = TestGroup.Time;
            NewTestGroup["TimeSum"] = TestGroup.TimeSum;
            NewTestGroup["Memory"] = TestGroup.Memory;
            NewTestGroup["TestCases"] = configor::json::array({});
            for (auto &TestCase : TestGroup.TestCases)
                NewTestGroup["TestCases"].push_back({{"TCID", TestCase.TCID},
                                                     {"Output", TestCase.Output},
                                                     {"StandardOutput", TestCase.StandardOutput},
                                                     {"StandardError", TestCase.StandardError},
                                                     {"Result", (int)TestCase.Result},
                                                     {"Description", TestCase.Description},
                                                     {"Time", TestCase.Time},
                                                     {"Memory", TestCase.Memory},
                                                     {"Score", TestCase.Score}});
            JSON.push_back(std::move(NewTestGroup));
        }
        JSONData = JSON.dump();
    }
    catch (std::exception &e)
    {
        CREATE_RESULT(false, e.what());
    }
    CREATE_RESULT(true, "Success");
}

RESULT SUBMISSIONS::GetSubmission(int SID, SUBMISSION &Submission)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Submissions")
                         .Select("PID")
                         .Select("UID")
                         .Select("Code")
                         .Select("Result")
                         .Select("Description")
                         .Select("Time")
                         .Select("TimeSum")
                         .Select("Memory")
                         .Select("Score")
                         .Select("EnableO2")
                         .Select("CreateTime")
                         .Select("TestGroups")
                         .Where("SID", SID)
                         .Execute(
                             [SID, &Submission](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "Submission not found");
                                 Submission.SID = SID;
                                 Submission.PID = Data[0]["PID"];
                                 Submission.UID = atoi(Data[0]["UID"].c_str());
                                 Submission.Code = Data[0]["Code"];
                                 Submission.Result = (JUDGE_RESULT)atoi(Data[0]["Result"].c_str());
                                 Submission.Description = Data[0]["Description"];
                                 Submission.Time = atoi(Data[0]["Time"].c_str());
                                 Submission.TimeSum = atoi(Data[0]["TimeSum"].c_str());
                                 Submission.Memory = atoi(Data[0]["Memory"].c_str());
                                 Submission.Score = atoi(Data[0]["Score"].c_str());
                                 Submission.EnableO2 = atoi(Data[0]["EnableO2"].c_str());
                                 Submission.CreateTime = UTILITIES::StringToTime(Data[0]["CreateTime"]);

                                 RETURN_IF_FAILED(PROBLEMS::GetProblem(Submission.PID, Submission.Problem))
                                 for (size_t TGID = 0; TGID < Submission.Problem.TestGroups.size(); TGID++)
                                 {
                                     TEST_GROUP TempTestGroup;
                                     TempTestGroup.PID = Submission.PID;
                                     TempTestGroup.SID = Submission.SID;
                                     TempTestGroup.TGID = TGID;
                                     for (size_t TCID = 0; TCID < Submission.Problem.TestGroups[TGID].TestCases.size(); TCID++)
                                     {
                                         TEST_CASE TempTestCase;
                                         TempTestCase.PID = Submission.PID;
                                         TempTestCase.SID = Submission.SID;
                                         TempTestCase.TGID = TGID;
                                         TempTestCase.TCID = TCID;
                                         TempTestCase.Problem = &Submission.Problem;
                                         TempTestCase.UnjudgedTestCase = &TempTestCase.Problem->TestGroups[TempTestCase.TGID].TestCases[TempTestCase.TCID];
                                         TempTestGroup.TestCases.push_back(TempTestCase);
                                     }
                                     Submission.TestGroups.push_back(TempTestGroup);
                                 }

                                 RETURN_IF_FAILED(JSONToTestGroups(Data[0]["TestGroups"], Submission.TestGroups, Submission.PID, Submission.SID));
                                 CREATE_RESULT(true, "Loading submission success");
                             }));
    CREATE_RESULT(true, "Loading submission success");
}
RESULT SUBMISSIONS::AddSubmission(SUBMISSION &Submission)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Submissions")
                         .Select("PID")
                         .Where("SID", Submission.SID)
                         .Execute(
                             [](auto Data)
                             {
                                 if (Data.size() != 0)
                                     CREATE_RESULT(false, "Submission already exists");
                                 CREATE_RESULT(true, "Submission not found");
                             }));
    std::string TestGroupsData;
    RETURN_IF_FAILED(TestGroupsToJSON(Submission.TestGroups, TestGroupsData));
    RETURN_IF_FAILED(DATABASE::INSERT("Submissions")
                         .Insert("PID", Submission.PID)
                         .Insert("UID", std::to_string(Submission.UID))
                         .Insert("Code", Submission.Code)
                         .Insert("Result", std::to_string(Submission.Result))
                         .Insert("Description", Submission.Description)
                         .Insert("Time", std::to_string(Submission.Time))
                         .Insert("TimeSum", std::to_string(Submission.TimeSum))
                         .Insert("Memory", std::to_string(Submission.Memory))
                         .Insert("Score", std::to_string(Submission.Score))
                         .Insert("EnableO2", std::to_string(Submission.EnableO2))
                         .Insert("TestGroups", TestGroupsData)
                         .Execute(
                             [&Submission](int SID)
                             {
                                 Submission.SID = SID;
                                 CREATE_RESULT(true, "Adding submission success");
                             }));
    CREATE_RESULT(true, "Adding submission success");
}
RESULT SUBMISSIONS::UpdateSubmission(SUBMISSION *Submission)
{
    std::string TestGroupsData;
    RETURN_IF_FAILED(TestGroupsToJSON(Submission->TestGroups, TestGroupsData));
    RETURN_IF_FAILED(DATABASE::UPDATE("Submissions")
                         .Set("PID", Submission->PID)
                         .Set("UID", Submission->UID)
                         .Set("Code", Submission->Code)
                         .Set("Result", Submission->Result)
                         .Set("Description", Submission->Description)
                         .Set("Time", Submission->Time)
                         .Set("TimeSum", Submission->TimeSum)
                         .Set("Memory", Submission->Memory)
                         .Set("Score", Submission->Score)
                         .Set("EnableO2", Submission->EnableO2)
                         .Set("TestGroups", TestGroupsData)
                         .Where("SID", Submission->SID)
                         .Execute());
    CREATE_RESULT(true, "Updating submission success");
}
RESULT SUBMISSIONS::UpdateTestGroup(TEST_GROUP *TestGroup)
{
    // SUBMISSION Submission;
    // RETURN_IF_FAILED(GetSubmission(TestGroup->SID, Submission));
    // Submission.TestGroups[TestGroup->TGID] = *TestGroup;
    // RETURN_IF_FAILED(UpdateSubmission(Submission));
    CREATE_RESULT(true, "Updating test group success");
}
RESULT SUBMISSIONS::UpdateTestCase(TEST_CASE *TestCase)
{
    // SUBMISSION Submission;
    // RETURN_IF_FAILED(GetSubmission(TestCase->SID, Submission));
    // Submission.TestGroups[TestCase->TGID].TestCases[TestCase->TCID] = *TestCase;
    // RETURN_IF_FAILED(UpdateSubmission(Submission));
    CREATE_RESULT(true, "Updating test case success");
}
