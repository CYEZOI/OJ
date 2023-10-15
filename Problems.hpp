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

#include "Result.hpp"
#include "Problem.hpp"

class PROBLEMS
{
private:
    static RESULT JSONToSamples(std::string JSONData, std::vector<SAMPLE> &Samples);
    static RESULT JSONToUnjudgedTestGroups(std::string JSONData, std::vector<TEST_GROUP_DATA> &UnjudgedTestGroups);
    static RESULT SamplesToJSON(std::vector<SAMPLE> Samples, std::string &JSONData);
    static RESULT UnjudgedTestGroupsToJSON(std::vector<TEST_GROUP_DATA> UnjudgedTestGroups, std::string &JSONData);

    friend class API_PROCEED;

public:
    static RESULT AddProblem(PROBLEM Problem);
    static RESULT GetProblem(std::string PID, PROBLEM &Problem);
    static RESULT UpdateProblem(PROBLEM Problem);
    static RESULT DeleteProblem(std::string PID);
};
