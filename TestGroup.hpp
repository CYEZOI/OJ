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

#pragma once

#include "Logger.hpp"
#include "TestCase.hpp"
#include <map>
#include <vector>

class TEST_GROUP {
  private:
    int TGID;
    std::string PID;
    int SID;
    int Score = 0;
    JUDGE_RESULT Result = JUDGE_RESULT::WAITING;
    std::vector<TEST_CASE> TestCases;
    int TestCasesPassed = 0;
    std::string IOFilename;
    int ResultCount[JUDGE_RESULT::REJECTED] = {0};
    int Time = 0;
    int TimeSum = 0;
    int Memory = 0;

    void UpdateAllResults(JUDGE_RESULT Result);

    friend class API_PROCEED;
    friend class PROBLEM;
    friend class PROBLEMS;
    friend class SUBMISSION;
    friend class SUBMISSIONS;
    friend class TEMP_TEST_DATA;

  public:
    void Judge();
};
