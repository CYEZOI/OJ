#include "Problems.hpp"
#include "Database.hpp"
#include "configor/json.hpp"

RESULT PROBLEMS::JSONToSamples(std::string JSONData, std::vector<SAMPLE> &Samples)
{
    Samples.clear();
    try
    {
        configor::json JSON = configor::json::parse(JSONData);
        for (auto &Sample : JSON)
        {
            SAMPLE NewSample;
            NewSample.Input = Sample["Input"].as_string();
            NewSample.Output = Sample["Output"].as_string();
            NewSample.Description = Sample["Description"].as_string();
            Samples.push_back(NewSample);
        }
    }
    catch (std::exception &e)
    {
        CREATE_RESULT(false, e.what());
    }
    CREATE_RESULT(true, "Success");
}
RESULT PROBLEMS::JSONToUnjudgedTestGroups(std::string JSONData, std::vector<TEST_GROUP_DATA> &UnjudgedTestGroups)
{
    UnjudgedTestGroups.clear();
    try
    {
        configor::json JSON = configor::json::parse(JSONData);
        for (auto &TestGroup : JSON)
        {
            TEST_GROUP_DATA NewTestGroup;
            NewTestGroup.TGID = TestGroup["TGID"].as_integer();
            NewTestGroup.TestCases.clear();
            for (auto &TestCase : TestGroup["TestCases"])
            {
                TEST_CASE_DATA NewTestCase;
                NewTestCase.TCID = TestCase["TCID"].as_integer();
                NewTestCase.Input = TestCase["Input"].as_string();
                NewTestCase.Answer = TestCase["Output"].as_string();
                NewTestCase.TimeLimit = TestCase["TimeLimit"].as_integer();
                NewTestCase.MemoryLimit = TestCase["MemoryLimit"].as_integer();
                NewTestCase.Score = TestCase["Score"].as_integer();
                NewTestGroup.TestCases.push_back(NewTestCase);
            }
            UnjudgedTestGroups.push_back(NewTestGroup);
        }
    }
    catch (std::exception &e)
    {
        CREATE_RESULT(false, e.what());
    }
    CREATE_RESULT(true, "Success");
}
RESULT PROBLEMS::SamplesToJSON(std::vector<SAMPLE> Samples, std::string &JSONData)
{
    try
    {
        configor::json JSON = configor::json::array({});
        for (auto &Sample : Samples)
            JSON.push_back({{"Input", Sample.Input},
                            {"Output", Sample.Output},
                            {"Description", Sample.Description}});
        JSONData = JSON.dump();
    }
    catch (std::exception &e)
    {
        CREATE_RESULT(false, e.what());
    }
    CREATE_RESULT(true, "Success");
}
RESULT PROBLEMS::UnjudgedTestGroupsToJSON(std::vector<TEST_GROUP_DATA> UnjudgedTestGroups, std::string &JSONData)
{
    try
    {
        configor::json JSON = configor::json::array({});
        for (auto &TestGroup : UnjudgedTestGroups)
        {
            configor::json NewTestGroup;
            NewTestGroup["TGID"] = TestGroup.TGID;
            NewTestGroup["TestCases"] = configor::json::array({});
            for (auto &TestCase : TestGroup.TestCases)
                NewTestGroup["TestCases"].push_back({{"TCID", TestCase.TCID},
                                                     {"Input", TestCase.Input},
                                                     {"Output", TestCase.Answer},
                                                     {"TimeLimit", TestCase.TimeLimit},
                                                     {"MemoryLimit", TestCase.MemoryLimit},
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

RESULT PROBLEMS::GetProblem(std::string PID, PROBLEM &Problem)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Problems")
                         .Select("Title")
                         .Select("IOFilename")
                         .Select("Description")
                         .Select("Input")
                         .Select("Output")
                         .Select("Range")
                         .Select("Hint")
                         .Select("Samples")
                         .Select("TestGroups")
                         .Where("PID", PID)
                         .Execute(
                             [PID, &Problem](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "Problem not found");
                                 Problem.PID = PID;
                                 Problem.Title = Data[0]["Title"];
                                 Problem.Description = Data[0]["Description"];
                                 Problem.Input = Data[0]["Input"];
                                 Problem.Output = Data[0]["Output"];
                                 Problem.Range = Data[0]["Range"];
                                 Problem.Hint = Data[0]["Hint"];
                                 RETURN_IF_FAILED(JSONToSamples(Data[0]["Samples"], Problem.Samples));
                                 RETURN_IF_FAILED(JSONToUnjudgedTestGroups(Data[0]["TestGroups"], Problem.TestGroups));
                                 Problem.IOFilename = Data[0]["IOFilename"];
                                 CREATE_RESULT(true, "Success");
                             }));
    CREATE_RESULT(true, "Success");
}
RESULT PROBLEMS::AddProblem(PROBLEM Problem)
{
    std::string SamplesJSON, TestGroupsJSON;
    RETURN_IF_FAILED(SamplesToJSON(Problem.Samples, SamplesJSON));
    RETURN_IF_FAILED(UnjudgedTestGroupsToJSON(Problem.TestGroups, TestGroupsJSON));
    RETURN_IF_FAILED(DATABASE::INSERT("Problems")
                         .Insert("PID", Problem.PID)
                         .Insert("Title", Problem.Title)
                         .Insert("IOFilename", Problem.IOFilename)
                         .Insert("Description", Problem.Description)
                         .Insert("Input", Problem.Input)
                         .Insert("Output", Problem.Output)
                         .Insert("Range", Problem.Range)
                         .Insert("Hint", Problem.Hint)
                         .Insert("Samples", SamplesJSON)
                         .Insert("TestGroups", TestGroupsJSON)
                         .Execute());
    CREATE_RESULT(true, "Success");
}
