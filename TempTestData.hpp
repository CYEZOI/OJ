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

#pragma once
#include "Submission.hpp"
class TEMP_TEST_DATA
{
public:
    static RESULT Insert(TEST_GROUP TestGroup);
    static RESULT Update(TEST_GROUP TestGroup);
    static RESULT Select(TEST_GROUP &TestGroup);
    static RESULT Delete(TEST_GROUP TestGroup);

    static RESULT Insert(TEST_CASE TestCase);
    static RESULT Update(TEST_CASE TestCase);
    static RESULT Select(TEST_CASE &TestCase);
    static RESULT Delete(TEST_CASE TestCase);
};
