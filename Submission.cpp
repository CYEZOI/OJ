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

#include "Submission.hpp"
#include "Problems.hpp"
#include "Settings.hpp"
#include "Submissions.hpp"
#include "TempTestData.hpp"
#include "Utilities.hpp"
#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

void SUBMISSION::UpdateAllResults(JUDGE_RESULT Result) {
    this->Result = Result;
    for (size_t i = 0; i < TestGroups.size(); i++)
        TestGroups[i].UpdateAllResults(Result);
}
void SUBMISSION::RedirectIO() {
    if (freopen((WorkDir + "/main.log").c_str(), "w", stdout) == nullptr)
        throw EXCEPTION("Can not open output data file");

    if (freopen((WorkDir + "/main.log").c_str(), "w", stderr) == nullptr)
        throw EXCEPTION("Can not open error data file");
}
void SUBMISSION::SetupEnvrionment() {
    const std::string DirsToMake[7] = {
        "./root",
        "./usr",
        "./etc",
        "./etc/alternatives",
        "./proc",
        "./tmp",
        "./dev"};
    for (int i = 0; i < 7; i++) {
        if (mkdir(DirsToMake[i].c_str(), 0755) == -1)
            throw EXCEPTION("Can not create dir for the new root");
    }

    if (chown("./root", JudgeUserID, JudgeUserGroupID) == -1)
        throw EXCEPTION("Can not change the owner of the new root");

    if (chown("./tmp", JudgeUserID, JudgeUserGroupID) == -1)
        throw EXCEPTION("Can not change the owner of the new root");

    if (mount("/usr", "./usr", "ext4", MS_BIND, nullptr) == -1)
        throw EXCEPTION("Can not mount the new root");

    if (mount("/usr", "./usr", "ext4", MS_BIND | MS_REMOUNT | MS_RDONLY, nullptr) == -1)
        throw EXCEPTION("Can not remount the new root");

    if (mount("/proc", "./proc", "proc", MS_BIND, nullptr) == -1)
        throw EXCEPTION("Can not mount the new root");

    if (mount("/dev", "./dev", "devtmpfs", MS_BIND, nullptr) == -1)
        throw EXCEPTION("Can not mount the new root");

    if (mount("/dev", "./dev", "devtmpfs", MS_BIND | MS_REMOUNT | MS_RDONLY, nullptr) == -1)
        throw EXCEPTION("Can not remount the new root");

    const std::string DirsToLink[5] = {
        "bin",
        "lib",
        "lib32",
        "libx32",
        "lib64"};
    for (int i = 0; i < 5; i++) {
        if (symlink(("/usr/" + DirsToLink[i]).c_str(), DirsToLink[i].c_str()) == -1)
            throw EXCEPTION("Can not create symlink for the new root");
    }

    UTILITIES::CopyDir("/etc/alternatives", "./etc/alternatives");

    if (chroot(WorkDir.c_str()) != 0)
        throw EXCEPTION("Can not change root dir");
}
void SUBMISSION::RemoveEnvrionment() {
    if (chdir(WorkDir.c_str()) == -1)
        throw EXCEPTION("Can not change directory");

    const std::string DirsToUmount[4] = {
        "usr",
        "proc",
        "dev",
        "usr"};
    for (int i = 0; i < 4; i++)
        umount(DirsToUmount[i].c_str());
    const std::string FilesToRemove[5] = {
        "bin",
        "lib",
        "lib32",
        "lib64",
        "libx32"};
    for (int i = 0; i < 5; i++)
        remove(FilesToRemove[i].c_str());

    const std::string DirsToRemove[6] = {
        "root",
        "etc",
        "dev",
        "proc",
        "tmp",
        "usr"};
    for (int i = 0; i < 6; i++)
        UTILITIES::RemoveDir(DirsToRemove[i].c_str());
}
void SUBMISSION::ChangeUser() {
    if (setgid(JudgeUserGroupID) != 0)
        throw EXCEPTION("Can not change gid");
    if (setuid(JudgeUserID) != 0)
        throw EXCEPTION("Can not change uid");
    if (setresgid(JudgeUserGroupID,
                  JudgeUserGroupID,
                  JudgeUserGroupID) != 0)
        throw EXCEPTION("Can not change real gid");
    if (setresuid(JudgeUserID,
                  JudgeUserID,
                  JudgeUserID) != 0)
        throw EXCEPTION("Can not change real uid");
}
void SUBMISSION::SetLimits() {
    struct rlimit Limit;
    Limit.rlim_cur = Limit.rlim_max = TimeLimit + 1;
    if (setrlimit(RLIMIT_CPU, &Limit))
        throw EXCEPTION("Can not set cpu limit");

    if (alarm(ceil(TimeLimit / 1000.0) + 1) != 0)
        throw EXCEPTION("Can not set alarm");

    Limit.rlim_max = OutputLimit + 1024 * 1024;
    Limit.rlim_cur = OutputLimit;
    if (setrlimit(RLIMIT_FSIZE, &Limit))
        throw EXCEPTION("Can not set file size limit");

    Limit.rlim_max = Limit.rlim_cur = 0;
    if (setrlimit(RLIMIT_CORE, &Limit))
        throw EXCEPTION("Can not set core limit");
}
void SUBMISSION::ChildProcess() {
    std::vector<std::string>
        CompileCommands = {Compiler,
                           "-fno-diagnostics-color",
                           "-fno-asm",
                           "-w",
                           "-lm",
                           "-fmax-errors=1",
                           "-std=c++14",
                           EnableO2 ? "-O2" : "-O0",
                           "--static",
                           "-DONLINE_JUDGE",
                           "-o",
                           "/main",
                           "/main.cpp"};

    if (nice(19) != 19)
        throw EXCEPTION("Can not change nice value");
    if (chdir(WorkDir.c_str()) == -1)
        throw EXCEPTION("Can not change to work dir");

    RedirectIO();
    SetupEnvrionment();
    ChangeUser();
    SetLimits();

    char **CompileArguments = new char *[CompileCommands.size() + 1];
    for (size_t i = 0; i < CompileCommands.size(); i++) {
        CompileArguments[i] = (char *)CompileCommands[i].c_str();
    }
    CompileArguments[CompileCommands.size()] = nullptr;

    execvp(Compiler.c_str(), CompileArguments);

    delete[] CompileArguments;
    throw EXCEPTION("Can not execute program");
}
void SUBMISSION::ParentProcess() {
    while (true) {
        int Status;
        struct rusage Usage;
        if (wait4(ProcessID, &Status, 0, &Usage) == -1) {
            ptrace(PTRACE_KILL, ProcessID, nullptr, nullptr);
            throw EXCEPTION("Can not wait for child process");
        }
        if (WIFEXITED(Status) || WIFSIGNALED(Status) || WIFSTOPPED(Status))
            break;
    }
}
void SUBMISSION::CreateFiles() {
    std::ofstream LogFile(WorkDir + "/main.log");
    if (!LogFile.is_open())
        throw EXCEPTION("Can not open data file");
    LogFile.close();

    if (chown((WorkDir + "/main.log").c_str(), JudgeUserID, JudgeUserGroupID) == -1)
        throw EXCEPTION("Can not change group of compile log file");

    if (chmod((WorkDir + "/main.log").c_str(), 0760) == -1)
        throw EXCEPTION("Can not change permission of compile log file");

    std::ofstream OutputFile(WorkDir + "/main");
    if (!OutputFile.is_open())
        throw EXCEPTION("Can not open data file");
    OutputFile.close();

    if (chown((WorkDir + "/main").c_str(), JudgeUserID, JudgeUserGroupID) == -1)
        throw EXCEPTION("Can not change group of compile output file");

    if (chmod((WorkDir + "/main").c_str(), 0760) == -1)
        throw EXCEPTION("Can not change permission of compile output file");
}
void SUBMISSION::Compile() {
    UpdateAllResults(JUDGE_RESULT::COMPILING);

    CreateFiles();

    pid_t PID = fork();
    if (PID == 0) {
        ChildProcess();
        exit(0);
    } else {
        this->ProcessID = PID;
        ParentProcess();
        RemoveEnvrionment();

        struct stat OutputFileStatus;

        if (stat((WorkDir + "/main").c_str(), &OutputFileStatus) < 0)
            throw EXCEPTION("Can not open check compile output file status");

        if (OutputFileStatus.st_size != 0) {
            UpdateAllResults(JUDGE_RESULT::COMPILED);
            return;
        }

        std::ifstream CompileLog(WorkDir + "/main.log");
        if (!CompileLog.is_open())
            throw EXCEPTION("Can not open compile output log file");

        std::string CompileResult;
        std::string Line;
        while (getline(CompileLog, Line))
            CompileResult += Line + "\n";
        CompileLog.close();

        if (CompileResult == "")
            throw EXCEPTION("Compiler output log file is empty but compile failed");
        UpdateAllResults(JUDGE_RESULT::COMPILATION_ERROR);
        Description = CompileResult;
    }
}
void SUBMISSION::RunTestGroups() {
    if (Result != JUDGE_RESULT::COMPILED)
        if (Problem.IOFilename == "")
            Problem.IOFilename = std::to_string(SID);
    for (size_t i = 0; i < TestGroups.size(); i++) {
        TEMP_TEST_DATA::Insert(TestGroups[i]);
        if (fork() == 0) {
            TestGroups[i].Judge();
            exit(0);
        }
    }
    bool Judged = false;
    while (!Judged) {
        Judged = true;
        for (size_t i = 0; i < TestGroups.size(); i++) {
            TEMP_TEST_DATA::Select(TestGroups[i]);
            if (TestGroups[i].Result >= JUDGE_RESULT::WAITING)
                Judged = false;
        }
        SUBMISSIONS::UpdateSubmission(*this);
        usleep(500'000);
    }
    for (size_t i = 0; i < TestGroups.size(); i++) {
        TEMP_TEST_DATA::Select(TestGroups[i]);
        TEMP_TEST_DATA::Delete(TestGroups[i]);
        Score += TestGroups[i].Score;
        ResultCount[TestGroups[i].Result]++;
        Time = std::max(Time, TestGroups[i].Time);
        TimeSum += TestGroups[i].TimeSum;
        Memory = std::max(Memory, TestGroups[i].Memory);
    }

    int MaxCount = 0;
    JUDGE_RESULT MaxResult = JUDGE_RESULT::UNKNOWN_ERROR;
    int SecondMaxCount = 0;
    JUDGE_RESULT SecondMaxResult = JUDGE_RESULT::UNKNOWN_ERROR;
    for (size_t i = 0; i < JUDGE_RESULT::REJECTED; i++)
        if (ResultCount[i] > MaxCount) {
            SecondMaxCount = MaxCount;
            SecondMaxResult = MaxResult;
            MaxCount = ResultCount[i];
            MaxResult = (JUDGE_RESULT)i;
        } else if (ResultCount[i] > SecondMaxCount) {
            SecondMaxCount = ResultCount[i];
            SecondMaxResult = (JUDGE_RESULT)i;
        }
    Result = (SecondMaxCount == 0 || MaxResult != JUDGE_RESULT::ACCEPTED) ? MaxResult : SecondMaxResult;
}

void SUBMISSION::Set(std::string Code, std::string PID) {
    UpdateAllResults(JUDGE_RESULT::WAITING);
    this->Code = Code;
    this->PID = PID;
    if (Code.length() > 1024 * 1024)
        throw EXCEPTION("Source code is too long");
}

void SUBMISSION::Judge() {
    if (Result != JUDGE_RESULT::WAITING)
        throw EXCEPTION("Submission has been judged");

    SETTINGS::GetSettings("JudgeUserID", JudgeUserID);
    SETTINGS::GetSettings("JudgeUserGroupID", JudgeUserGroupID);
    SETTINGS::GetSettings("Compiler", Compiler);
    SETTINGS::GetSettings("JudgeUsername", JudgeUsername);
    SETTINGS::GetSettings("CompileTimeLimit", TimeLimit);
    SETTINGS::GetSettings("CompileOutputLimit", OutputLimit);

    UpdateAllResults(JUDGE_RESULT::FETCHED);

    WorkDir = "/home/" + JudgeUsername + "/Run/" + std::to_string(SID);
    UTILITIES::MakeDir(WorkDir);

    std::ofstream SourceFile = std::ofstream(WorkDir + "/main.cpp");
    if (!SourceFile.is_open())
        throw EXCEPTION("Can not open source file to write");
    SourceFile << Code;
    SourceFile.close();

    try {
        Compile();
        if (Result == JUDGE_RESULT::COMPILED)
            RunTestGroups();
    } catch (EXCEPTION ErrorData) {
        UpdateAllResults(JUDGE_RESULT::SYSTEM_ERROR);
        Description = ErrorData.Message;
    }

    UTILITIES::RemoveDir(WorkDir);
    SUBMISSIONS::UpdateSubmission(*this);
}
