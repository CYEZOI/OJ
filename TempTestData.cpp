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

#include "TempTestData.hpp"
#include "Database.hpp"
void TEMP_TEST_DATA::Insert(TEST_GROUP TestGroup) {
    configor::json::array_type TestCasesJSON;
    for (size_t i = 0; i < TestGroup.TestCases.size(); i++) {
        TestCasesJSON.push_back(configor::json({{"Result", (int)TestGroup.TestCases[i].Result},
                                                {"Description", TestGroup.TestCases[i].Description},
                                                {"Time", TestGroup.TestCases[i].Time},
                                                {"Memory", TestGroup.TestCases[i].Memory},
                                                {"Score", TestGroup.TestCases[i].Score}}));
    }
    return DATABASE::INSERT("TempTestData")
        .Insert("Index", std::to_string(TestGroup.SID) + "-" + std::to_string(TestGroup.TGID))
        .Insert("Data",
                configor::json({{"Result", (int)TestGroup.Result},
                                {"Score", TestGroup.Score},
                                {"Time", TestGroup.Time},
                                {"TimeSum", TestGroup.TimeSum},
                                {"Memory", TestGroup.Memory},
                                {"TestCases", TestCasesJSON}})
                    .dump())
        .Execute();
}
void TEMP_TEST_DATA::Update(TEST_GROUP TestGroup) {
    configor::json::array_type TestCasesJSON;
    for (size_t i = 0; i < TestGroup.TestCases.size(); i++) {
        TestCasesJSON.push_back(configor::json({{"Result", (int)TestGroup.TestCases[i].Result},
                                                {"Description", TestGroup.TestCases[i].Description},
                                                {"Time", TestGroup.TestCases[i].Time},
                                                {"Memory", TestGroup.TestCases[i].Memory},
                                                {"Score", TestGroup.TestCases[i].Score}}));
    }
    return DATABASE::UPDATE("TempTestData")
        .Set("Data",
             configor::json({{"Result", (int)TestGroup.Result},
                             {"Score", TestGroup.Score},
                             {"Time", TestGroup.Time},
                             {"TimeSum", TestGroup.TimeSum},
                             {"Memory", TestGroup.Memory},
                             {"TestCases", TestCasesJSON}})
                 .dump())
        .Where("Index", std::to_string(TestGroup.SID) + "-" + std::to_string(TestGroup.TGID))
        .Execute();
}
void TEMP_TEST_DATA::Select(TEST_GROUP &TestGroup) {
    return DATABASE::SELECT("TempTestData")
        .Select("Data")
        .Where("Index", std::to_string(TestGroup.SID) + "-" + std::to_string(TestGroup.TGID))
        .Execute(
            [&TestGroup](auto Data) {
                configor::json JSONData = configor::json::parse(Data[0]["Data"]);
                TestGroup.Result = (JUDGE_RESULT)JSONData["Result"].as_integer();
                TestGroup.Score = JSONData["Score"].as_integer();
                TestGroup.Time = JSONData["Time"].as_integer();
                TestGroup.TimeSum = JSONData["TimeSum"].as_integer();
                TestGroup.Memory = JSONData["Memory"].as_integer();
                for (size_t i = 0; i < JSONData["TestCases"].size(); i++) {
                    TestGroup.TestCases[i].Result = (JUDGE_RESULT)JSONData["TestCases"][i]["Result"].as_integer();
                    TestGroup.TestCases[i].Description = JSONData["TestCases"][i]["Description"].as_string();
                    TestGroup.TestCases[i].Time = JSONData["TestCases"][i]["Time"].as_integer();
                    TestGroup.TestCases[i].Memory = JSONData["TestCases"][i]["Memory"].as_integer();
                    TestGroup.TestCases[i].Score = JSONData["TestCases"][i]["Score"].as_integer();
                }
            });
}
void TEMP_TEST_DATA::Delete(TEST_GROUP TestGroup) {
    return DATABASE::DELETE("TempTestData")
        .Where("Index", std::to_string(TestGroup.SID) + "-" + std::to_string(TestGroup.TGID))
        .Execute();
}

void TEMP_TEST_DATA::Insert(TEST_CASE TestCase) {
    return DATABASE::INSERT("TempTestData")
        .Insert("Index", std::to_string(TestCase.SID) + "-" + std::to_string(TestCase.TGID) + "-" + std::to_string(TestCase.TCID))
        .Insert("Data",
                configor::json({{"Result", (int)TestCase.Result},
                                {"Description", TestCase.Description},
                                {"Time", TestCase.Time},
                                {"Memory", TestCase.Memory},
                                {"Score", TestCase.Score}})
                    .dump())
        .Execute();
}
void TEMP_TEST_DATA::Update(TEST_CASE TestCase) {
    return DATABASE::UPDATE("TempTestData")
        .Set("Data",
             configor::json({{"Result", (int)TestCase.Result},
                             {"Description", TestCase.Description},
                             {"Time", TestCase.Time},
                             {"Memory", TestCase.Memory},
                             {"Score", TestCase.Score}})
                 .dump())
        .Where("Index", std::to_string(TestCase.SID) + "-" + std::to_string(TestCase.TGID) + "-" + std::to_string(TestCase.TCID))
        .Execute();
}
void TEMP_TEST_DATA::Select(TEST_CASE &TestCase) {
    return DATABASE::SELECT("TempTestData")
        .Select("Data")
        .Where("Index", std::to_string(TestCase.SID) + "-" + std::to_string(TestCase.TGID) + "-" + std::to_string(TestCase.TCID))
        .Execute(
            [&TestCase](auto Data) {
                configor::json JSONData = configor::json::parse(Data[0]["Data"]);
                TestCase.Result = (JUDGE_RESULT)JSONData["Result"].as_integer();
                TestCase.Description = JSONData["Description"].as_string();
                TestCase.Time = JSONData["Time"].as_integer();
                TestCase.Memory = JSONData["Memory"].as_integer();
                TestCase.Score = JSONData["Score"].as_integer();
            });
}
void TEMP_TEST_DATA::Delete(TEST_CASE TestCase) {
    return DATABASE::DELETE("TempTestData")
        .Where("Index", std::to_string(TestCase.SID) + "-" + std::to_string(TestCase.TGID) + "-" + std::to_string(TestCase.TCID))
        .Execute();
}
