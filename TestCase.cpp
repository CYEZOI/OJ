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

#include "TestCase.hpp"
#include "Logger.hpp"
#include "Settings.hpp"
#include "Submissions.hpp"
#include "TempTestData.hpp"
#include "Utilities.hpp"
#include <fstream>
#include <map>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

void TEST_CASE::RedirectIO() {
    if (freopen((WorkDir + "/" + Problem->IOFilename + ".in").c_str(), "r", stdin) == nullptr)
        throw EXCEPTION("Can not open input data file");

    if (Problem->IOFilename == "") {
        if (freopen((WorkDir + "/" + Problem->IOFilename + ".out").c_str(), "w", stdout) == nullptr)
            throw EXCEPTION("Can not open output data file");
    } else {
        if (freopen((WorkDir + "/std.out").c_str(), "w", stdout) == nullptr)
            throw EXCEPTION("Can not open output data file");
    }

    if (freopen((WorkDir + "/std.err").c_str(), "w", stderr) == nullptr)
        throw EXCEPTION("Can not open error data file");
}
void TEST_CASE::SetupEnvrionment() {
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
void TEST_CASE::RemoveEnvrionment() {
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
void TEST_CASE::ChangeUser() {
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
void TEST_CASE::SetLimits() {
    struct rlimit Limit;
    Limit.rlim_cur = Limit.rlim_max = UnjudgedTestCase->TimeLimit + 1;
    if (setrlimit(RLIMIT_CPU, &Limit))
        throw EXCEPTION("Can not set cpu limit");

    if (alarm(ceil(UnjudgedTestCase->TimeLimit / 1000.0) + 1) != 0)
        throw EXCEPTION("Can not set alarm");

    Limit.rlim_max = UnjudgedTestCase->MemoryLimit + 1024 * 1024;
    Limit.rlim_cur = UnjudgedTestCase->MemoryLimit;
    if (setrlimit(RLIMIT_AS, &Limit))
        throw EXCEPTION("Can not set memory limit");

    Limit.rlim_max = 32 * 1024 * 1024 + 1024 * 1024;
    Limit.rlim_cur = 32 * 1024 * 1024;
    if (setrlimit(RLIMIT_FSIZE, &Limit))
        throw EXCEPTION("Can not set file size limit");

    Limit.rlim_max = Limit.rlim_cur = 1;
    if (setrlimit(RLIMIT_NPROC, &Limit))
        throw EXCEPTION("Can not set process limit");

    Limit.rlim_max = 256 * 1024 * 1024 + 1024 * 1024;
    Limit.rlim_cur = 256 * 1024 * 1024;
    if (setrlimit(RLIMIT_STACK, &Limit))
        throw EXCEPTION("Can not set stack limit");

    Limit.rlim_max = Limit.rlim_cur = 0;
    if (setrlimit(RLIMIT_CORE, &Limit))
        throw EXCEPTION("Can not set core limit");
}
void TEST_CASE::ChildProcess() {
    if (nice(19) != 19)
        throw EXCEPTION("Can not change nice value");
    if (chdir(WorkDir.c_str()) == -1)
        throw EXCEPTION("Can not change to work dir");

    RedirectIO();
    SetupEnvrionment();
    ChangeUser();
    SetLimits();

    if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1)
        throw EXCEPTION("Can not trace self");

    execl("/main", "main", nullptr);

    throw EXCEPTION("Can not execute program");
}
bool TEST_CASE::CheckSignal() {
    int Status;
    struct rusage Usage;
    if (wait4(ProcessID, &Status, 0, &Usage) == -1)
        throw EXCEPTION("Can not wait for child process");
    Time = (Usage.ru_utime.tv_sec * 1000 + Usage.ru_utime.tv_usec / 1000) +
           (Usage.ru_stime.tv_sec * 1000 + Usage.ru_stime.tv_usec / 1000);
    if (WIFEXITED(Status)) {
        int ExitCode = WEXITSTATUS(Status);
        if (ExitCode != 0) {
            Result = JUDGE_RESULT::RUNTIME_ERROR;
            Description = "Child process exited with code " + std::to_string(ExitCode) + " which is recognized as runtime error";
            return false;
        }
        Result = JUDGE_RESULT::JUDGED;
        return false;
    }
    if (WIFSIGNALED(Status)) {
        int Signal = WTERMSIG(Status);
        if (Signal == SIGVTALRM || Signal == SIGALRM || Signal == SIGXCPU) {
            Result = JUDGE_RESULT::TIME_LIMIT_EXCEEDED;
            return false;
        } else if (Signal == SIGXFSZ) {
            Result = JUDGE_RESULT::OUTPUT_LIMIT_EXCEEDED;
            return false;
        } else if (Signal == SIGSEGV) {
            Result = JUDGE_RESULT::MEMORY_LIMIT_EXCEEDED;
            return false;
        } else {
            Result = JUDGE_RESULT::RUNTIME_ERROR;
            Description = "Received an unknown signal";
            return false;
        }
    }
    if (WIFSTOPPED(Status)) {
        int Signal = WSTOPSIG(Status);
        if (Signal == SIGTRAP) {
            if (ptrace(PTRACE_SYSCALL, ProcessID, nullptr, nullptr) == -1)
                throw EXCEPTION("Can not continue child process");
        } else {
            if (ptrace(PTRACE_SYSCALL, ProcessID, nullptr, Signal) == -1)
                throw EXCEPTION("Can not continue child process");
        }
    }
    return true;
}
bool TEST_CASE::CheckMemory() {
    std::ifstream ProcessStatus("/proc/" + std::to_string(ProcessID) + "/status");
    if (!ProcessStatus.is_open())
        throw EXCEPTION("Can not open process status file");
    std::string Line;
    while (std::getline(ProcessStatus, Line))
        if (Line.substr(0, 6) == "VmPeak") {
            Memory = std::max(Memory, std::stoi(Line.substr(7, Line.find("kB") - 7)) * 1024);
            if (Memory > UnjudgedTestCase->MemoryLimit) {
                Result = JUDGE_RESULT::MEMORY_LIMIT_EXCEEDED;
                return false;
            }
            break;
        }
    ProcessStatus.close();
    return true;
}
bool TEST_CASE::CheckSystemCall() {
    struct user_regs_struct Regs;
    if (ptrace(PTRACE_GETREGS, ProcessID, nullptr, &Regs) == -1)
        return true;
    int CallID = (unsigned int)Regs.REG_SYSCALL % SystemCallList.size();
    if (SystemCallList[CallID] == 0) {
        Result = JUDGE_RESULT::RESTRICTED_FUNCTION;
        Description = "Child process tried to execute system call " + std::to_string(CallID);
        kill(ProcessID, SIGKILL);
        return false;
    } else
        SystemCallList[CallID]--;
    if (ptrace(PTRACE_SYSCALL, ProcessID, nullptr, nullptr) != 0)
        throw EXCEPTION("Can not trace system calls");
    return true;
}
void TEST_CASE::ParentProcess() {
    while (true) {
        if (!CheckSignal())
            break;
        if (!CheckMemory())
            break;
        if (!CheckSystemCall())
            break;
    }
}
void TEST_CASE::Run() {
    std::ofstream InputFile(WorkDir + "/" + Problem->IOFilename + ".in");
    if (!InputFile.is_open())
        throw EXCEPTION("Can not open data file");
    InputFile << UnjudgedTestCase->Input;
    InputFile.close();

    if (chown((WorkDir + "/" + Problem->IOFilename + ".in").c_str(), JudgeUserID, JudgeUserGroupID) == -1)
        throw EXCEPTION("Can not change group of input file");

    if (chmod((WorkDir + "/" + Problem->IOFilename + ".in").c_str(), 0740) == -1)
        throw EXCEPTION("Can not change permission of input file");

    std::ofstream OutputFile(WorkDir + "/" + Problem->IOFilename + ".out");
    if (!OutputFile.is_open())
        throw EXCEPTION("Can not open data file");
    OutputFile.close();

    if (chown((WorkDir + "/" + Problem->IOFilename + ".out").c_str(), JudgeUserID, JudgeUserGroupID) == -1)
        throw EXCEPTION("Can not change group of output file");

    if (chmod((WorkDir + "/" + Problem->IOFilename + ".out").c_str(), 0760) == -1)
        throw EXCEPTION("Can not change permission of output file");

    std::ofstream StandardOutputFile(WorkDir + "/std.out");
    if (!StandardOutputFile.is_open())
        throw EXCEPTION("Can not open data file");
    StandardOutputFile.close();

    std::ofstream StandardErrorFile(WorkDir + "/std.err");
    if (!StandardErrorFile.is_open())
        throw EXCEPTION("Can not open data file");
    StandardErrorFile.close();

    pid_t ProcessID = fork();
    if (ProcessID == 0) {
        ChildProcess();
        exit(0);
    } else {
        this->ProcessID = ProcessID;
        ParentProcess();
        kill(ProcessID, SIGKILL);
        RemoveEnvrionment();
    }
}
void TEST_CASE::Compare() {
    if (Result != JUDGE_RESULT::JUDGED)

        Result = JUDGE_RESULT::COMPARING;

    std::string Line;
    std::ifstream OutputFile(WorkDir + "/" + Problem->IOFilename + ".out");
    if (!OutputFile.is_open())
        throw EXCEPTION("Can not open data file");
    while (std::getline(OutputFile, Line))
        Output += Line + "\n";
    OutputFile.close();

    std::ifstream StandardOutputFile(WorkDir + "/std.out");
    if (!StandardOutputFile.is_open())
        throw EXCEPTION("Can not open data file");
    while (std::getline(StandardOutputFile, Line))
        StandardOutput += Line + "\n";
    StandardOutputFile.close();

    std::ifstream StandardErrorFile(WorkDir + "/std.err");
    if (!StandardErrorFile.is_open())
        throw EXCEPTION("Can not open data file");
    while (std::getline(StandardErrorFile, Line))
        StandardError += Line + "\n";
    StandardErrorFile.close();

    Output = UTILITIES::RemoveSpaces(Output);
    UnjudgedTestCase->Answer = UTILITIES::RemoveSpaces(UnjudgedTestCase->Answer);
    StandardOutput = UTILITIES::RemoveSpaces(StandardOutput);
    StandardError = UTILITIES::RemoveSpaces(StandardError);

    std::string FixedOutput = UTILITIES::StringReplaceAll(Output, "\r", "");
    FixedOutput = UTILITIES::StringReplaceAll(FixedOutput, "\n", "");
    FixedOutput = UTILITIES::StringReplaceAll(FixedOutput, "\t", "");
    FixedOutput = UTILITIES::StringReplaceAll(FixedOutput, " ", "");
    std::string FixedAnswer = UTILITIES::StringReplaceAll(UnjudgedTestCase->Answer, "\r", "");
    FixedAnswer = UTILITIES::StringReplaceAll(FixedAnswer, "\n", "");
    FixedAnswer = UTILITIES::StringReplaceAll(FixedAnswer, "\t", "");
    FixedAnswer = UTILITIES::StringReplaceAll(FixedAnswer, " ", "");

    if (StandardError != "") {
        Result = JUDGE_RESULT::RUNTIME_ERROR;
        Description = "Do not output to stderr if you do so. ";
    } else if (Output == UnjudgedTestCase->Answer)
        Result = JUDGE_RESULT::ACCEPTED;
    else {
        if (FixedOutput == FixedAnswer)
            Result = JUDGE_RESULT::PRESENTATION_ERROR;
        else if (Output == "" && StandardOutput != "") {
            Result = JUDGE_RESULT::RUNTIME_ERROR;
            Description = "Your program outputs data to stdout instead of a file";
        } else if (Output == "" && StandardOutput == "") {
            Result = JUDGE_RESULT::RUNTIME_ERROR;
            Description = "Output is empty, checking if the program output it's data to a wrong file";
        } else
            Result = JUDGE_RESULT::WRONG_ANSWER;
    }
}

void TEST_CASE::Judge() {
    SETTINGS::GetSettings("JudgeUserID", JudgeUserID);
    SETTINGS::GetSettings("JudgeUserGroupID", JudgeUserGroupID);
    SETTINGS::GetSettings("JudgeUsername", JudgeUsername);
    std::string SystemCallListString;
    SETTINGS::GetSettings("SystemCallList", SystemCallListString);
    std::vector<std::string> TempSystemCallList = UTILITIES::StringSplit(SystemCallListString, ",");
    for (auto SystemCall : TempSystemCallList)
        SystemCallList.push_back(std::stoi(SystemCall));

    WorkDir = "/home/" + JudgeUsername + "/Run/" + std::to_string(SID) + "-" + std::to_string(TGID) + "-" + std::to_string(TCID);
    UTILITIES::MakeDir(WorkDir);
    UTILITIES::CopyFile("/home/" + JudgeUsername + "/Run/" + std::to_string(SID) + "/main", WorkDir + "/main");

    bool UpdateDatabaseSignal = true;
    std::thread UpdateDatabase(
        [this, &UpdateDatabaseSignal]() {
            while (UpdateDatabaseSignal) {
                TEMP_TEST_DATA::Update(*this);
                usleep(500'000);
            }
        });

    Result = JUDGE_RESULT::JUDGING;

    try {
        Run();
        if (Time > UnjudgedTestCase->TimeLimit) {
            Result = JUDGE_RESULT::TIME_LIMIT_EXCEEDED;
            UpdateDatabaseSignal = false;
            UpdateDatabase.join();
            TEMP_TEST_DATA::Update(*this);
        }
        kill(ProcessID, SIGKILL);
        waitpid(ProcessID, nullptr, 0);
        if (Result == JUDGE_RESULT::JUDGED)
            Compare();
    } catch (EXCEPTION ErrorData) {
        Result = JUDGE_RESULT::SYSTEM_ERROR;
        Description = ErrorData.Message;
        UpdateDatabaseSignal = false;
        UpdateDatabase.join();
        TEMP_TEST_DATA::Update(*this);
    }

    Score = Result == JUDGE_RESULT::ACCEPTED ? Score : 0;
    UTILITIES::RemoveDir(WorkDir);

    UpdateDatabaseSignal = false;
    UpdateDatabase.join();

    TEMP_TEST_DATA::Update(*this);
}
