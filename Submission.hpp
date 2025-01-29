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

#pragma once

#include "Logger.hpp"
#include "Problem.hpp"
#include "TestGroup.hpp"
#include <vector>

class SUBMISSION {
  private:
    int SID = 0;
    int UID = 0;
    int ProcessID = 0;
    int Score = 0;
    int TestGroupsPassed = 0;
    std::string WorkDir;
    std::string Code;
    std::string PID;
    JUDGE_RESULT Result = JUDGE_RESULT::UNKNOWN_ERROR;
    std::string Description;
    std::vector<TEST_GROUP> TestGroups;
    int ResultCount[JUDGE_RESULT::REJECTED] = {0};
    int Time = 0;
    int TimeSum = 0;
    int Memory = 0;
    bool EnableO2 = false;
    time_t CreateTime;
    PROBLEM Problem;

    int JudgeUserID = 0;
    int JudgeUserGroupID = 0;
    std::string Compiler = "";
    std::string JudgeUsername = "";
    int TimeLimit = 1000;
    int OutputLimit = 1024 * 1024 * 1024;

    void UpdateAllResults(JUDGE_RESULT Result);
    void RedirectIO();
    void SetupEnvrionment();
    void RemoveEnvrionment();
    void ChangeUser();
    void SetLimits();
    void ChildProcess();
    void ParentProcess();
    void CreateFiles();
    void Compile();
    void RunTestGroups();

    friend class API_PROCEED;
    friend class JUDGING_LIST;
    friend class SUBMISSIONS;

  public:
    void Set(std::string Code, std::string PID);
    void Judge();
};
