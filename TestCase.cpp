#include "Utilities.hpp"
#include "Logger.hpp"
#include "Settings.hpp"
#include "TestCase.hpp"
#include "Submissions.hpp"
#include <map>
#include <fstream>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/ptrace.h>
#include <sys/resource.h>

RESULT TEST_CASE::RedirectIO()
{
    if (freopen((WorkDir + "/" + Problem->IOFilename + ".in").c_str(), "r", stdin) == nullptr)
        CREATE_RESULT(false, "Can not open input data file")

    if (Problem->IOFilename == std::to_string(SID))
    {
        if (freopen((WorkDir + "/" + Problem->IOFilename + ".out").c_str(), "w", stdout) == nullptr)
            CREATE_RESULT(false, "Can not open output data file");
    }
    else
    {
        if (freopen((WorkDir + "/std.out").c_str(), "w", stdout) == nullptr)
            CREATE_RESULT(false, "Can not open output data file")
    }

    if (freopen((WorkDir + "/std.err").c_str(), "w", stderr) == nullptr)
        CREATE_RESULT(false, "Can not open error data file")

    CREATE_RESULT(true, "Redirected IO")
}
RESULT TEST_CASE::SetupEnvrionment()
{
    const std::string DirsToMake[7] = {
        "./root",
        "./usr",
        "./etc",
        "./etc/alternatives",
        "./proc",
        "./tmp",
        "./dev"};
    for (int i = 0; i < 7; i++)
    {
        if (mkdir(DirsToMake[i].c_str(), 0755) == -1)
            CREATE_RESULT(false, "Can not create dir for the new root")
    }

    if (chown("./root", Settings.GetJudgeUserID(), Settings.GetJudgeUserGroupID()) == -1)
        CREATE_RESULT(false, "Can not change the owner of the new root")

    if (chown("./tmp", Settings.GetJudgeUserID(), Settings.GetJudgeUserGroupID()) == -1)
        CREATE_RESULT(false, "Can not change the owner of the new root")

    if (mount("/usr", "./usr", "ext4", MS_BIND, nullptr) == -1)
        CREATE_RESULT(false, "Can not mount the new root")

    if (mount("/usr", "./usr", "ext4", MS_BIND | MS_REMOUNT | MS_RDONLY, nullptr) == -1)
        CREATE_RESULT(false, "Can not remount the new root")

    if (mount("/proc", "./proc", "proc", MS_BIND, nullptr) == -1)
        CREATE_RESULT(false, "Can not mount the new root")

    if (mount("/dev", "./dev", "devtmpfs", MS_BIND, nullptr) == -1)
        CREATE_RESULT(false, "Can not mount the new root")

    if (mount("/dev", "./dev", "devtmpfs", MS_BIND | MS_REMOUNT | MS_RDONLY, nullptr) == -1)
        CREATE_RESULT(false, "Can not remount the new root")

    const std::string DirsToLink[5] = {
        "bin",
        "lib",
        "lib32",
        "libx32",
        "lib64"};
    for (int i = 0; i < 5; i++)
    {
        if (symlink(("/usr/" + DirsToLink[i]).c_str(), DirsToLink[i].c_str()) == -1)
            CREATE_RESULT(false, "Can not create symlink for the new root")
    }

    RETURN_IF_FAILED(UTILITIES::CopyDir("/etc/alternatives", "./etc/alternatives"))

    if (chroot(WorkDir.c_str()) != 0)
        CREATE_RESULT(false, "Can not change root dir")

    CREATE_RESULT(true, "Environment setted")
}
RESULT TEST_CASE::RemoveEnvrionment()
{
    if (chdir(WorkDir.c_str()) == -1)
        CREATE_RESULT(false, "Can not change directory")

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
        RETURN_IF_FAILED(UTILITIES::RemoveDir(DirsToRemove[i].c_str()))

    CREATE_RESULT(true, "Environment removed")
}
RESULT TEST_CASE::ChangeUser()
{
    if (setgid(Settings.GetJudgeUserGroupID()) != 0)
        CREATE_RESULT(false, "Can not change gid")
    if (setuid(Settings.GetJudgeUserID()) != 0)
        CREATE_RESULT(false, "Can not change uid")
    if (setresgid(Settings.GetJudgeUserGroupID(),
                  Settings.GetJudgeUserGroupID(),
                  Settings.GetJudgeUserGroupID()) != 0)
        CREATE_RESULT(false, "Can not change real gid")
    if (setresuid(Settings.GetJudgeUserID(),
                  Settings.GetJudgeUserID(),
                  Settings.GetJudgeUserID()) != 0)
        CREATE_RESULT(false, "Can not change real uid")

    CREATE_RESULT(true, "Changed uid and gid")
}
RESULT TEST_CASE::SetLimits()
{
    struct rlimit Limit;
    Limit.rlim_cur = Limit.rlim_max = UnjudgedTestCase->TimeLimit + 1;
    if (setrlimit(RLIMIT_CPU, &Limit))
        CREATE_RESULT(false, "Can not set cpu limit")

    if (alarm(ceil(UnjudgedTestCase->TimeLimit / 1000.0) + 1) != 0)
        CREATE_RESULT(false, "Can not set alarm")

    Limit.rlim_max = UnjudgedTestCase->MemoryLimit + 1024 * 1024;
    Limit.rlim_cur = UnjudgedTestCase->MemoryLimit;
    if (setrlimit(RLIMIT_AS, &Limit))
        CREATE_RESULT(false, "Can not set memory limit")

    Limit.rlim_max = 32 * 1024 * 1024 + 1024 * 1024;
    Limit.rlim_cur = 32 * 1024 * 1024;
    if (setrlimit(RLIMIT_FSIZE, &Limit))
        CREATE_RESULT(false, "Can not set file size limit")

    Limit.rlim_max = Limit.rlim_cur = 1;
    if (setrlimit(RLIMIT_NPROC, &Limit))
        CREATE_RESULT(false, "Can not set process limit")

    Limit.rlim_max = 256 * 1024 * 1024 + 1024 * 1024;
    Limit.rlim_cur = 256 * 1024 * 1024;
    if (setrlimit(RLIMIT_STACK, &Limit))
        CREATE_RESULT(false, "Can not set stack limit")

    Limit.rlim_max = Limit.rlim_cur = 0;
    if (setrlimit(RLIMIT_CORE, &Limit))
        CREATE_RESULT(false, "Can not set core limit")

    CREATE_RESULT(true, "Set limits")
}
RESULT TEST_CASE::ChildProcess()
{

    if (nice(19) != 19)
        CREATE_RESULT(false, "Can not change nice value")
    if (chdir(WorkDir.c_str()) == -1)
        CREATE_RESULT(false, "Can not change to work dir")

    RETURN_IF_FAILED(RedirectIO())
    RETURN_IF_FAILED(SetupEnvrionment())
    RETURN_IF_FAILED(ChangeUser())
    RETURN_IF_FAILED(SetLimits())

    if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1)
        CREATE_RESULT(false, "Can not trace self")

    execl("/main", "main", nullptr);

    CREATE_RESULT(false, "Can not execute program");
}
RESULT TEST_CASE::CheckSignal()
{
    int Status;
    struct rusage Usage;
    if (wait4(ProcessID, &Status, 0, &Usage) == -1)
        CREATE_RESULT(false, "Can not wait for child process")
    RETURN_IF_FAILED(SetTime((Usage.ru_utime.tv_sec * 1000 + Usage.ru_utime.tv_usec / 1000) +
                             (Usage.ru_stime.tv_sec * 1000 + Usage.ru_stime.tv_usec / 1000)));
    if (WIFEXITED(Status))
    {
        int ExitCode = WEXITSTATUS(Status);
        if (ExitCode != 0)
        {
            RETURN_IF_FAILED(SetResult(JUDGE_RESULT::RUNTIME_ERROR))
            RETURN_IF_FAILED(SetDescription("Child process exited with code " + std::to_string(ExitCode) + " which is recognized as runtime error"))
            CREATE_RESULT(false, "Child process exited with code " + std::to_string(ExitCode))
        }
        RETURN_IF_FAILED(SetResult(JUDGE_RESULT::JUDGED))
        CREATE_RESULT(false, "Judged")
    }
    if (WIFSIGNALED(Status))
    {
        int Signal = WTERMSIG(Status);
        if (Signal == SIGVTALRM || Signal == SIGALRM || Signal == SIGXCPU)
        {
            RETURN_IF_FAILED(SetResult(JUDGE_RESULT::TIME_LIMIT_EXCEEDED))
            CREATE_RESULT(false, "Time limit exceeded");
        }
        else if (Signal == SIGXFSZ)
        {
            RETURN_IF_FAILED(SetResult(JUDGE_RESULT::OUTPUT_LIMIT_EXCEEDED))
            CREATE_RESULT(false, "Output limit exceeded");
        }
        else if (Signal == SIGSEGV)
        {
            RETURN_IF_FAILED(SetResult(JUDGE_RESULT::MEMORY_LIMIT_EXCEEDED))
            CREATE_RESULT(false, "Memory limit exceeded");
        }
        else
        {
            RETURN_IF_FAILED(SetResult(JUDGE_RESULT::RUNTIME_ERROR))
            RETURN_IF_FAILED(SetDescription("Received an unknown signal"));
            CREATE_RESULT(false, "Can not recognize signal");
        }
        RETURN_IF_FAILED(SetResult(JUDGE_RESULT::JUDGED))
        CREATE_RESULT(false, "Got signal");
    }
    if (WIFSTOPPED(Status))
    {
        int Signal = WSTOPSIG(Status);
        if (Signal == SIGTRAP)
        {
            if (ptrace(PTRACE_SYSCALL, ProcessID, nullptr, nullptr) == -1)
                CREATE_RESULT(false, "Can not continue child process");
        }
        else
        {
            if (ptrace(PTRACE_SYSCALL, ProcessID, nullptr, Signal) == -1)
                CREATE_RESULT(false, "Can not continue child process");
        }
    }
    CREATE_RESULT(true, "No signal");
}
RESULT TEST_CASE::CheckMemory()
{
    std::ifstream ProcessStatus("/proc/" + std::to_string(ProcessID) + "/status");
    if (!ProcessStatus.is_open())
        CREATE_RESULT(false, "Can not open process status file");
    std::string Line;
    while (std::getline(ProcessStatus, Line))
        if (Line.substr(0, 6) == "VmPeak")
        {
            RETURN_IF_FAILED(SetMemory(std::max(Memory, std::stoi(Line.substr(7, Line.find("kB") - 7)))))
            if (Memory > UnjudgedTestCase->MemoryLimit)
            {
                RETURN_IF_FAILED(SetResult(JUDGE_RESULT::MEMORY_LIMIT_EXCEEDED))
                CREATE_RESULT(false, "Memory limit exceeded");
            }
            break;
        }
    ProcessStatus.close();
    CREATE_RESULT(true, "Memory is OK");
}
RESULT TEST_CASE::CheckSystemCall()
{
    struct user_regs_struct Regs;
    if (ptrace(PTRACE_GETREGS, ProcessID, nullptr, &Regs) == -1)
        CREATE_RESULT(true, "Can not get registers")
    int CallID = (unsigned int)Regs.orig_rax % Settings.SystemCallCount;
    SystemCallCount[CallID]++;
    if (Settings.IsBannedSystemCall(CallID, SystemCallCount[CallID]))
    {
        RETURN_IF_FAILED(SetResult(JUDGE_RESULT::RESTRICTED_FUNCTION))
        RETURN_IF_FAILED(SetDescription("Child process tried to execute system call " + std::to_string(CallID) + (CallID == 14 ? ", which may because of memory exceed" : "")))
        CREATE_RESULT(false, "The system call is banned")
    }
    if (ptrace(PTRACE_SYSCALL, ProcessID, nullptr, nullptr) != 0)
        CREATE_RESULT(false, "Can not trace system calls")
    CREATE_RESULT(true, "No banned system call");
}
RESULT TEST_CASE::ParentProcess()
{
    while (true)
    {
        if (Result != JUDGE_RESULT::JUDGING)
            CREATE_RESULT(true, "Judge stopped because of status is not JUDGING")

        RETURN_IF_FAILED(CheckSignal())
        RETURN_IF_FAILED(CheckMemory())
        RETURN_IF_FAILED(CheckSystemCall())
    }
    CREATE_RESULT(false, "Shouldn't go here");
}
RESULT TEST_CASE::Run()
{
    std::ofstream InputFile(WorkDir + "/" + Problem->IOFilename + ".in");
    if (!InputFile.is_open())
        CREATE_RESULT(false, "Can not open data file")
    InputFile << UnjudgedTestCase->Input;
    InputFile.close();

    if (chown((WorkDir + "/" + Problem->IOFilename + ".in").c_str(), Settings.GetJudgeUserID(), Settings.GetJudgeUserGroupID()) == -1)
        CREATE_RESULT(false, "Can not change group of input file")

    if (chmod((WorkDir + "/" + Problem->IOFilename + ".in").c_str(), 0740) == -1)
        CREATE_RESULT(false, "Can not change permission of input file")

    std::ofstream OutputFile(WorkDir + "/" + Problem->IOFilename + ".out");
    if (!OutputFile.is_open())
        CREATE_RESULT(false, "Can not open data file")
    OutputFile.close();

    if (chown((WorkDir + "/" + Problem->IOFilename + ".out").c_str(), Settings.GetJudgeUserID(), Settings.GetJudgeUserGroupID()) == -1)
        CREATE_RESULT(false, "Can not change group of output file")

    if (chmod((WorkDir + "/" + Problem->IOFilename + ".out").c_str(), 0760) == -1)
        CREATE_RESULT(false, "Can not change permission of output file")

    std::ofstream StandardOutputFile(WorkDir + "/std.out");
    if (!StandardOutputFile.is_open())
        CREATE_RESULT(false, "Can not open data file")
    StandardOutputFile.close();

    std::ofstream StandardErrorFile(WorkDir + "/std.err");
    if (!StandardErrorFile.is_open())
        CREATE_RESULT(false, "Can not open data file")
    StandardErrorFile.close();

    pid_t ProcessID = fork();
    if (ProcessID == 0)
    {
        RETURN_IF_FAILED(ChildProcess())
        exit(0);
    }
    else
    {
        this->ProcessID = ProcessID;
        ParentProcess();
        RemoveEnvrionment();
    }

    CREATE_RESULT(true, "Run ended");
}
RESULT TEST_CASE::Compare()
{
    if (Result != JUDGE_RESULT::JUDGED)
        CREATE_RESULT(true, "Judge stopped because of status is not JUDGED")

    RETURN_IF_FAILED(SetResult(JUDGE_RESULT::COMPARING))

    std::string Line;
    std::ifstream OutputFile(WorkDir + "/" + Problem->IOFilename + ".out");
    if (!OutputFile.is_open())
        CREATE_RESULT(false, "Can not open data file")
    while (std::getline(OutputFile, Line))
        Output += Line + "\n";
    OutputFile.close();

    std::ifstream StandardOutputFile(WorkDir + "/std.out");
    if (!StandardOutputFile.is_open())
        CREATE_RESULT(false, "Can not open data file")
    while (std::getline(StandardOutputFile, Line))
        StandardOutput += Line + "\n";
    StandardOutputFile.close();

    std::ifstream StandardErrorFile(WorkDir + "/std.err");
    if (!StandardErrorFile.is_open())
        CREATE_RESULT(false, "Can not open data file")
    while (std::getline(StandardErrorFile, Line))
        StandardError += Line + "\n";
    StandardErrorFile.close();

    RETURN_IF_FAILED(SetOutput(UTILITIES::RemoveSpaces(Output)))
    UnjudgedTestCase->Answer = UTILITIES::RemoveSpaces(UnjudgedTestCase->Answer);
    RETURN_IF_FAILED(SetStandardOutput(UTILITIES::RemoveSpaces(StandardOutput)))
    RETURN_IF_FAILED(SetStandardError(UTILITIES::RemoveSpaces(StandardError)))

    std::string FixedOutput = UTILITIES::StringReplaceAll(Output, "\r", "");
    FixedOutput = UTILITIES::StringReplaceAll(FixedOutput, "\n", "");
    FixedOutput = UTILITIES::StringReplaceAll(FixedOutput, "\t", "");
    FixedOutput = UTILITIES::StringReplaceAll(FixedOutput, " ", "");
    std::string FixedAnswer = UTILITIES::StringReplaceAll(UnjudgedTestCase->Answer, "\r", "");
    FixedAnswer = UTILITIES::StringReplaceAll(FixedAnswer, "\n", "");
    FixedAnswer = UTILITIES::StringReplaceAll(FixedAnswer, "\t", "");
    FixedAnswer = UTILITIES::StringReplaceAll(FixedAnswer, " ", "");

    if (StandardError != "")
    {
        RETURN_IF_FAILED(SetResult(JUDGE_RESULT::RUNTIME_ERROR))
        RETURN_IF_FAILED(SetDescription("Do not output to stderr if you do so. "))
    }
    else if (Output == UnjudgedTestCase->Answer)
    {
        RETURN_IF_FAILED(SetResult(JUDGE_RESULT::ACCEPTED))
        RETURN_IF_FAILED(SetDescription("Accepted"))
    }
    else
    {
        if (FixedOutput == FixedAnswer)
        {
            RETURN_IF_FAILED(SetResult(JUDGE_RESULT::PRESENTATION_ERROR))
            RETURN_IF_FAILED(SetDescription("Presentation error"))
        }
        else if (Output == "" && StandardOutput != "")
        {
            RETURN_IF_FAILED(SetResult(JUDGE_RESULT::RUNTIME_ERROR))
            RETURN_IF_FAILED(SetDescription("Your program outputs data to stdout instead of a file"))
        }
        else if (Output == "" && StandardOutput == "")
        {
            RETURN_IF_FAILED(SetResult(JUDGE_RESULT::RUNTIME_ERROR))
            RETURN_IF_FAILED(SetDescription("Output is empty, checking if the program output it's data to a wrong file"))
        }
        else
        {
            RETURN_IF_FAILED(SetResult(JUDGE_RESULT::WRONG_ANSWER))
            RETURN_IF_FAILED(SetDescription("Wrong answer"))
        }
    }
    CREATE_RESULT(true, "Compared");
}

RESULT TEST_CASE::Judge()
{
    WorkDir = Settings.GetRunDir() + "/" + std::to_string(SID) + "-" + std::to_string(TGID) + "-" + std::to_string(TCID);
    RETURN_IF_FAILED(UTILITIES::MakeDir(WorkDir))
    RETURN_IF_FAILED(UTILITIES::CopyFile(Settings.GetRunDir() + "/" + std::to_string(SID) + "/main", WorkDir + "/main"))

    RETURN_IF_FAILED(SetResult(JUDGE_RESULT::JUDGING))

    RESULT RunResult = Run();
    if (Time > UnjudgedTestCase->TimeLimit)
    {
        RETURN_IF_FAILED(SetResult(JUDGE_RESULT::TIME_LIMIT_EXCEEDED))
        CREATE_RESULT(true, "Time limit exceeded")
    }
    kill(ProcessID, SIGKILL);
    waitpid(ProcessID, nullptr, 0);
    if (!RunResult.Success && Result == JUDGE_RESULT::JUDGING)
    {
        RETURN_IF_FAILED(SetResult(JUDGE_RESULT::SYSTEM_ERROR))
        RETURN_IF_FAILED(SetDescription(RunResult.Message))
        CREATE_RESULT(true, "Judged with system error from run")
    }

    RESULT CompareResult = Compare();
    if (!CompareResult.Success && Result == JUDGE_RESULT::JUDGING)
    {
        RETURN_IF_FAILED(SetResult(JUDGE_RESULT::SYSTEM_ERROR))
        RETURN_IF_FAILED(SetDescription(CompareResult.Message))
        CREATE_RESULT(true, "Judged with system error from compare")
    }

    RETURN_IF_FAILED(SetScore(Result == JUDGE_RESULT::ACCEPTED ? Score : 0))
    RETURN_IF_FAILED(UTILITIES::RemoveDir(WorkDir));

    CREATE_RESULT(true, "Judged")
}

RESULT TEST_CASE::SetOutput(std::string Output)
{
    this->Output = Output;
    RETURN_IF_FAILED(SUBMISSIONS::UpdateTestCase(this))
    CREATE_RESULT(true, "Output set")
}
RESULT TEST_CASE::SetStandardOutput(std::string StandardOutput)
{
    this->StandardOutput = StandardOutput;
    RETURN_IF_FAILED(SUBMISSIONS::UpdateTestCase(this))
    CREATE_RESULT(true, "Standard output set")
}
RESULT TEST_CASE::SetStandardError(std::string StandardError)
{
    this->StandardError = StandardError;
    RETURN_IF_FAILED(SUBMISSIONS::UpdateTestCase(this))
    CREATE_RESULT(true, "Standard error set")
}
RESULT TEST_CASE::SetResult(JUDGE_RESULT Result)
{
    this->Result = Result;
    RETURN_IF_FAILED(SUBMISSIONS::UpdateTestCase(this))
    CREATE_RESULT(true, "Result set")
}
RESULT TEST_CASE::SetDescription(std::string Description)
{
    this->Description = Description;
    RETURN_IF_FAILED(SUBMISSIONS::UpdateTestCase(this))
    CREATE_RESULT(true, "Description set")
}
RESULT TEST_CASE::SetScore(int Score)
{
    this->Score = Score;
    RETURN_IF_FAILED(SUBMISSIONS::UpdateTestCase(this))
    CREATE_RESULT(true, "Score set")
}
RESULT TEST_CASE::SetTime(int Time)
{
    this->Time = Time;
    RETURN_IF_FAILED(SUBMISSIONS::UpdateTestCase(this))
    CREATE_RESULT(true, "Time set")
}
RESULT TEST_CASE::SetMemory(int Memory)
{
    this->Memory = Memory;
    RETURN_IF_FAILED(SUBMISSIONS::UpdateTestCase(this))
    CREATE_RESULT(true, "Memory set")
}
