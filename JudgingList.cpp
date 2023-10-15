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

#include "JudgingList.hpp"
#include "Submissions.hpp"
#include "Settings.hpp"
#include "Problems.hpp"
#include <thread>
#include <unistd.h>
#include <dirent.h>

void JUDGING_LIST::Init()
{
    new std::thread(
        [this]()
        {
            while (true)
            {
                while (JudgingList.empty())
                    usleep(500'000);
                SUBMISSION Submission;
                OUTPUT_IF_FAILED(SUBMISSIONS::GetSubmission(JudgingList.front(), Submission))
                JudgingList.pop();
                OUTPUT_IF_FAILED(Submission.Judge())
            }
        });
}

RESULT JUDGING_LIST::Add(SUBMISSION &Submission)
{
    JudgingList.push(Submission.SID);
    CREATE_RESULT(true, "Submission added to judging list")
}

JUDGING_LIST JudgingList;
