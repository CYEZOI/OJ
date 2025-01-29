/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2025  langningchen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**********************************************************************/

#include "Submissions.hpp"
#include "Problems.hpp"
#include "Utilities.hpp"
#include "configor/json.hpp"

void SUBMISSIONS::JSONToTestGroups(std::string JSONData, std::vector<TEST_GROUP> &TestGroups, std::string PID, int SID) {
    try {
        configor::json JSON = configor::json::parse(JSONData);
        for (auto TestGroup : JSON) {
            size_t TGID = TestGroup["TGID"].as_integer();
            while (TestGroups.size() <= TGID)
                TestGroups.push_back({});
            TestGroups[TGID].TGID = TGID;
            TestGroups[TGID].PID = PID;
            TestGroups[TGID].SID = SID;
            TestGroups[TGID].Result = (JUDGE_RESULT)TestGroup["Result"].as_integer();
            TestGroups[TGID].Time = TestGroup["Time"].as_integer();
            TestGroups[TGID].TimeSum = TestGroup["TimeSum"].as_integer();
            TestGroups[TGID].Memory = TestGroup["Memory"].as_integer();
            TestGroups[TGID].Score = TestGroup["Score"].as_integer();
            for (auto TestCase : TestGroup["TestCases"]) {
                size_t TCID = TestCase["TCID"].as_integer();
                while (TestGroups[TGID].TestCases.size() <= TCID)
                    TestGroups[TGID].TestCases.push_back({});
                TestGroups[TGID].TestCases[TCID].TCID = TCID;
                TestGroups[TGID].TestCases[TCID].PID = PID;
                TestGroups[TGID].TestCases[TCID].SID = SID;
                TestGroups[TGID].TestCases[TCID].Result = (JUDGE_RESULT)TestCase["Result"].as_integer();
                TestGroups[TGID].TestCases[TCID].Description = TestCase["Description"].as_string();
                TestGroups[TGID].TestCases[TCID].Time = TestCase["Time"].as_integer();
                TestGroups[TGID].TestCases[TCID].Memory = TestCase["Memory"].as_integer();
                TestGroups[TGID].TestCases[TCID].Score = TestCase["Score"].as_integer();
            }
        }
    } catch (std::exception &e) {
        throw EXCEPTION(e.what());
    }
}
void SUBMISSIONS::TestGroupsToJSON(std::vector<TEST_GROUP> TestGroups, std::string &JSONData) {
    try {
        configor::json JSON = configor::json::array({});
        for (auto &TestGroup : TestGroups) {
            configor::json NewTestGroup;
            NewTestGroup["TGID"] = TestGroup.TGID;
            NewTestGroup["Result"] = (int)TestGroup.Result;
            NewTestGroup["Time"] = TestGroup.Time;
            NewTestGroup["TimeSum"] = TestGroup.TimeSum;
            NewTestGroup["Memory"] = TestGroup.Memory;
            NewTestGroup["Score"] = TestGroup.Score;
            NewTestGroup["TestCases"] = configor::json::array({});
            for (auto &TestCase : TestGroup.TestCases)
                NewTestGroup["TestCases"].push_back({{"TCID", TestCase.TCID},
                                                     {"Result", (int)TestCase.Result},
                                                     {"Description", TestCase.Description},
                                                     {"Time", TestCase.Time},
                                                     {"Memory", TestCase.Memory},
                                                     {"Score", TestCase.Score}});
            JSON.push_back(std::move(NewTestGroup));
        }
        JSONData = JSON.dump();
    } catch (std::exception &e) {
        throw EXCEPTION(e.what());
    }
}

void SUBMISSIONS::AddSubmission(SUBMISSION &Submission) {
    DATABASE::SELECT("Submissions")
        .Select("PID")
        .Where("SID", Submission.SID)
        .Execute(
            [](auto Data) {
                if (Data.size() != 0)
                    throw EXCEPTION("Submission already exists");
            });
    std::string TestGroupsData;
    TestGroupsToJSON(Submission.TestGroups, TestGroupsData);
    DATABASE::INSERT("Submissions")
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
            [&Submission](int SID) {
                Submission.SID = SID;
            });
}
void SUBMISSIONS::GetSubmission(int SID, SUBMISSION &Submission) {
    DATABASE::SELECT("Submissions")
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
            [SID, &Submission](auto Data) {
                if (Data.size() == 0)
                    throw EXCEPTION("Submission not found");
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

                PROBLEMS::GetProblem(Submission.PID, Submission.Problem);
                for (size_t TGID = 0; TGID < Submission.Problem.TestGroups.size(); TGID++) {
                    TEST_GROUP TempTestGroup;
                    TempTestGroup.PID = Submission.PID;
                    TempTestGroup.SID = Submission.SID;
                    TempTestGroup.TGID = TGID;
                    for (size_t TCID = 0; TCID < Submission.Problem.TestGroups[TGID].TestCases.size(); TCID++) {
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

                JSONToTestGroups(Data[0]["TestGroups"], Submission.TestGroups, Submission.PID, Submission.SID);
            });
}
void SUBMISSIONS::UpdateSubmission(SUBMISSION Submission) {
    std::string TestGroupsData;
    TestGroupsToJSON(Submission.TestGroups, TestGroupsData);
    DATABASE::UPDATE("Submissions")
        .Set("PID", Submission.PID)
        .Set("UID", Submission.UID)
        .Set("Code", Submission.Code)
        .Set("Result", Submission.Result)
        .Set("Description", Submission.Description)
        .Set("Time", Submission.Time)
        .Set("TimeSum", Submission.TimeSum)
        .Set("Memory", Submission.Memory)
        .Set("Score", Submission.Score)
        .Set("EnableO2", Submission.EnableO2)
        .Set("TestGroups", TestGroupsData)
        .Where("SID", Submission.SID)
        .Execute();
}
void SUBMISSIONS::DeleteSubmission(int SID) {
    DATABASE::DELETE("Submissions")
        .Where("SID", SID)
        .Execute();
}
