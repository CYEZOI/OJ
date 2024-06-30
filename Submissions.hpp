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

#include "Database.hpp"
#include "Submission.hpp"

class SUBMISSIONS {
  private:
    static void JSONToTestGroups(std::string JSONData, std::vector<TEST_GROUP> &TestGroups, std::string PID, int SID);
    static void TestGroupsToJSON(std::vector<TEST_GROUP> TestGroups, std::string &JSONData);

    friend class API_PROCEED;

  public:
    static void AddSubmission(SUBMISSION &Submission);
    static void GetSubmission(int SID, SUBMISSION &Submission);
    static void UpdateSubmission(SUBMISSION Submission);
    static void DeleteSubmission(int SID);
};
