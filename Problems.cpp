/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2024  langningchen

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

#include "Problems.hpp"
#include "Database.hpp"
#include "configor/json.hpp"

void PROBLEMS::JSONToSamples(std::string JSONData, std::vector<SAMPLE> &Samples) {
    Samples.clear();
    try {
        configor::json JSON = configor::json::parse(JSONData);
        for (auto &Sample : JSON) {
            SAMPLE NewSample;
            NewSample.Input = Sample["Input"].as_string();
            NewSample.Output = Sample["Output"].as_string();
            NewSample.Description = Sample["Description"].as_string();
            Samples.push_back(NewSample);
        }
    } catch (std::exception &e) {
        throw EXCEPTION(e.what());
    }
}
void PROBLEMS::JSONToUnjudgedTestGroups(std::string JSONData, std::vector<TEST_GROUP_DATA> &UnjudgedTestGroups) {
    UnjudgedTestGroups.clear();
    try {
        configor::json JSON = configor::json::parse(JSONData);
        for (auto &TestGroup : JSON) {
            TEST_GROUP_DATA NewTestGroup;
            NewTestGroup.TGID = TestGroup["TGID"].as_integer();
            NewTestGroup.TestCases.clear();
            for (auto &TestCase : TestGroup["TestCases"]) {
                TEST_CASE_DATA NewTestCase;
                NewTestCase.TCID = TestCase["TCID"].as_integer();
                NewTestCase.Input = TestCase["Input"].as_string();
                NewTestCase.Answer = TestCase["Answer"].as_string();
                NewTestCase.TimeLimit = TestCase["TimeLimit"].as_integer();
                NewTestCase.MemoryLimit = TestCase["MemoryLimit"].as_integer();
                NewTestCase.Score = TestCase["Score"].as_integer();
                NewTestGroup.TestCases.push_back(NewTestCase);
            }
            UnjudgedTestGroups.push_back(NewTestGroup);
        }
    } catch (std::exception &e) {
        throw EXCEPTION(e.what());
    }
}
void PROBLEMS::SamplesToJSON(std::vector<SAMPLE> Samples, std::string &JSONData) {
    try {
        configor::json JSON = configor::json::array({});
        for (auto &Sample : Samples)
            JSON.push_back({{"Input", Sample.Input},
                            {"Output", Sample.Output},
                            {"Description", Sample.Description}});
        JSONData = JSON.dump();
    } catch (std::exception &e) {
        throw EXCEPTION(e.what());
    }
}
void PROBLEMS::UnjudgedTestGroupsToJSON(std::vector<TEST_GROUP_DATA> UnjudgedTestGroups, std::string &JSONData) {
    try {
        configor::json JSON = configor::json::array({});
        for (auto &TestGroup : UnjudgedTestGroups) {
            configor::json NewTestGroup;
            NewTestGroup["TGID"] = TestGroup.TGID;
            NewTestGroup["TestCases"] = configor::json::array({});
            for (auto &TestCase : TestGroup.TestCases)
                NewTestGroup["TestCases"].push_back({{"TCID", TestCase.TCID},
                                                     {"Input", TestCase.Input},
                                                     {"Answer", TestCase.Answer},
                                                     {"TimeLimit", TestCase.TimeLimit},
                                                     {"MemoryLimit", TestCase.MemoryLimit},
                                                     {"Score", TestCase.Score}});
            JSON.push_back(std::move(NewTestGroup));
        }
        JSONData = JSON.dump();
    } catch (std::exception &e) {
        throw EXCEPTION(e.what());
    }
}

void PROBLEMS::AddProblem(PROBLEM Problem) {
    std::string SamplesJSON, TestGroupsJSON;
    SamplesToJSON(Problem.Samples, SamplesJSON);
    UnjudgedTestGroupsToJSON(Problem.TestGroups, TestGroupsJSON);
    DATABASE::INSERT("Problems")
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
        .Execute();
}
void PROBLEMS::GetProblem(std::string PID, PROBLEM &Problem) {
    DATABASE::SELECT("Problems")
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
            [PID, &Problem](auto Data) {
                if (Data.size() == 0)
                    throw EXCEPTION("Problem not found");
                Problem.PID = PID;
                Problem.Title = Data[0]["Title"];
                Problem.Description = Data[0]["Description"];
                Problem.Input = Data[0]["Input"];
                Problem.Output = Data[0]["Output"];
                Problem.Range = Data[0]["Range"];
                Problem.Hint = Data[0]["Hint"];
                JSONToSamples(Data[0]["Samples"], Problem.Samples);
                JSONToUnjudgedTestGroups(Data[0]["TestGroups"], Problem.TestGroups);
                Problem.IOFilename = Data[0]["IOFilename"];
            });
}
void PROBLEMS::UpdateProblem(PROBLEM Problem) {
    std::string SamplesJSON, TestGroupsJSON;
    SamplesToJSON(Problem.Samples, SamplesJSON);
    UnjudgedTestGroupsToJSON(Problem.TestGroups, TestGroupsJSON);
    DATABASE::UPDATE("Problems")
        .Set("Title", Problem.Title)
        .Set("IOFilename", Problem.IOFilename)
        .Set("Description", Problem.Description)
        .Set("Input", Problem.Input)
        .Set("Output", Problem.Output)
        .Set("Range", Problem.Range)
        .Set("Hint", Problem.Hint)
        .Set("Samples", SamplesJSON)
        .Set("TestGroups", TestGroupsJSON)
        .Where("PID", Problem.PID)
        .Execute();
}
void PROBLEMS::DeleteProblem(std::string PID) {
    DATABASE::DELETE("Problems")
        .Where("PID", PID)
        .Execute();
}
