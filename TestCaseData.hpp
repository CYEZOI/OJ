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

#include <string>

class TEST_CASE_DATA {
  private:
    int TCID = 0;
    std::string InputFilename;
    std::string AnswerFilename;
    int TimeLimit;
    int MemoryLimit;
    int Score;

    friend class PROBLEMS;
    friend class SUBMISSIONS;
    friend class TEST_CASE;
    friend class API_PROCEED;
    friend class TEST_GROUP_DATA;
};
