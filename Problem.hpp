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

#include "Sample.hpp"
#include "TestGroupData.hpp"

class PROBLEM {
  private:
    std::string PID;
    std::string Title;
    std::string Description;
    std::string Input;
    std::string Output;
    std::vector<SAMPLE> Samples;
    std::vector<TEST_GROUP_DATA> TestGroups;
    std::string Range;
    std::string Hint;
    std::string WorkDir;
    std::string IOFilename;

    friend class PROBLEM_LIST;
    friend class API_PROCEED;
    friend class SUBMISSION;
    friend class PROBLEMS;
    friend class SUBMISSIONS;
    friend class TEST_CASE;

  public:
    bool
    operator<(const PROBLEM &Compare) const;
};
