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

#include "Exception.hpp"
#include "Problem.hpp"

class PROBLEMS
{
private:
    static void JSONToSamples(std::string JSONData, std::vector<SAMPLE> &Samples);
    static void JSONToUnjudgedTestGroups(std::string JSONData, std::vector<TEST_GROUP_DATA> &UnjudgedTestGroups);
    static void SamplesToJSON(std::vector<SAMPLE> Samples, std::string &JSONData);
    static void UnjudgedTestGroupsToJSON(std::vector<TEST_GROUP_DATA> UnjudgedTestGroups, std::string &JSONData);

    friend class API_PROCEED;

public:
    static void AddProblem(PROBLEM Problem);
    static void GetProblem(std::string PID, PROBLEM &Problem);
    static void UpdateProblem(PROBLEM Problem);
    static void DeleteProblem(std::string PID);
};
