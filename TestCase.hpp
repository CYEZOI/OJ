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

#include "Exception.hpp"
#include "JudgeResult.hpp"
#include "Logger.hpp"
#include "Problem.hpp"
#include "Settings.hpp"

class TEST_CASE {
  private:
    int TCID = 0;
    pid_t ProcessID = 0;
    std::string Output;
    std::string StandardOutput;
    std::string StandardError;
    int SID = 0;
    std::string PID;
    int TGID = 0;
    std::string WorkDir;
    JUDGE_RESULT Result = JUDGE_RESULT::WAITING;
    std::string Description;
    int Time = 0;
    int Memory = 0;
    int Score = 0;
    PROBLEM *Problem;
    TEST_CASE_DATA *UnjudgedTestCase;

    int JudgeUserID = 0;
    int JudgeUserGroupID = 0;
    std::string JudgeUsername = "";
    std::vector<int> SystemCallList;

    void RedirectIO();
    void SetupEnvrionment();
    void RemoveEnvrionment();
    void ChangeUser();
    void SetLimits();
    void ChildProcess();
    bool CheckSignal();
    bool CheckMemory();
    bool CheckSystemCall();
    void ParentProcess();
    void Run();
    void Compare();

    friend class TEST_GROUP;
    friend class API_PROCEED;
    friend class PROBLEMS;
    friend class SUBMISSIONS;
    friend class SUBMISSION;
    friend class TEMP_TEST_DATA;

  public:
    void Judge();
};

#ifdef __arm__
struct user_regs_struct {
    long uregs[18];
};
#define REG_SYSCALL uregs[7]
#endif

#ifdef __aarch64__
#define PTRACE_GETREGS PTRACE_GETREGSET
#define REG_SYSCALL regs[18]
#endif

#ifdef __mips__
struct user_regs_struct {
    unsigned long long uregs[38];
};
#define REG_SYSCALL uregs[2]
#endif

#ifdef __i386
#define REG_SYSCALL orig_eax
#endif

#ifdef __x86_64__
#define REG_SYSCALL orig_rax
#endif
