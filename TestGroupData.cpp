/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2023  langningchen

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

#include "TestGroupData.hpp"

void TEST_GROUP_DATA::AddTestCase(std::string Input, std::string Answer, int TimeLimit, int MemoryLimit, int Score)
{
    TEST_CASE_DATA NewTestCase;
    NewTestCase.TCID = TestCases.size();
    NewTestCase.Input = Input;
    NewTestCase.Answer = Answer;
    NewTestCase.TimeLimit = TimeLimit;
    NewTestCase.MemoryLimit = MemoryLimit;
    NewTestCase.Score = Score;
    TestCases.push_back(NewTestCase);
}
