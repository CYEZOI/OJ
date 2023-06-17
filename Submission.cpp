#include "Settings.hpp"
#include "Submission.hpp"
#include "Utilities.hpp"
#include "Problems.hpp"
#include "Submissions.hpp"
#include <fstream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>
#include <thread>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/ptrace.h>
#include <sys/resource.h>

RESULT SUBMISSION::UpdateAllResults(JUDGE_RESULT Result)
{
    this->Result = Result;
    for (size_t i = 0; i < TestGroups.size(); i++)
        RETURN_IF_FAILED(TestGroups[i].UpdateAllResults(Result));
    CREATE_RESULT(true, "Update all results success")
}
RESULT SUBMISSION::RedirectIO()
{
    if (freopen((WorkDir + "/main.log").c_str(), "w", stdout) == nullptr)
        CREATE_RESULT(false, "Can not open output data file")

    if (freopen((WorkDir + "/main.log").c_str(), "w", stderr) == nullptr)
        CREATE_RESULT(false, "Can not open error data file")

    CREATE_RESULT(true, "Redirected IO success")
}
RESULT SUBMISSION::SetupEnvrionment()
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
RESULT SUBMISSION::RemoveEnvrionment()
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
RESULT SUBMISSION::ChangeUser()
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
RESULT SUBMISSION::SetLimits()
{
    int TimeLimit = Settings.GetCompileTimeLimit();
    int OutputLimit = Settings.GetCompileOutputLimit();
    struct rlimit Limit;
    Limit.rlim_cur = Limit.rlim_max = TimeLimit + 1;
    if (setrlimit(RLIMIT_CPU, &Limit))
        CREATE_RESULT(false, "Can not set cpu limit")

    if (alarm(ceil(TimeLimit / 1000.0) + 1) != 0)
        CREATE_RESULT(false, "Can not set alarm")

    Limit.rlim_max = OutputLimit + 1024 * 1024;
    Limit.rlim_cur = OutputLimit;
    if (setrlimit(RLIMIT_FSIZE, &Limit))
        CREATE_RESULT(false, "Can not set file size limit")

    Limit.rlim_max = Limit.rlim_cur = 0;
    if (setrlimit(RLIMIT_CORE, &Limit))
        CREATE_RESULT(false, "Can not set core limit")

    CREATE_RESULT(true, "Set limits")
}
RESULT SUBMISSION::ChildProcess()
{

    std::vector<std::string> CompileCommands = {Settings.GetCompiler(),
                                                "-fno-diagnostics-color",
                                                "-fno-asm",
                                                "-w",
                                                "-lm",
                                                "-fmax-errors=1",
                                                "-std=c++14",
                                                EnableO2 ? "-O2" : "-O1",
                                                "--static",
                                                "-DONLINE_JUDGE",
                                                "-o",
                                                "/main",
                                                "/main.cpp"};

    if (nice(19) != 19)
        CREATE_RESULT(false, "Can not change nice value")
    if (chdir(WorkDir.c_str()) == -1)
        CREATE_RESULT(false, "Can not change to work dir")

    RETURN_IF_FAILED(RedirectIO())
    RETURN_IF_FAILED(SetupEnvrionment())
    RETURN_IF_FAILED(ChangeUser())
    RETURN_IF_FAILED(SetLimits())

    char **CompileArguments = new char *[CompileCommands.size() + 1];
    for (size_t i = 0; i < CompileCommands.size(); i++)
    {
        CompileArguments[i] = (char *)CompileCommands[i].c_str();
    }
    CompileArguments[CompileCommands.size()] = nullptr;
    // execlp("ls", "ls", "-l", "/usr/lib/gcc/x86_64-linux-gnu/9/", nullptr);
    execvp(Settings.GetCompiler().c_str(), CompileArguments);

    CREATE_RESULT(false, "Can not execute program")
}
RESULT SUBMISSION::ParentProcess()
{
    while (true)
    {
        int Status;
        struct rusage Usage;
        if (wait4(ProcessID, &Status, 0, &Usage) == -1)
        {
            ptrace(PTRACE_KILL, ProcessID, nullptr, nullptr);
            CREATE_RESULT(false, "Can not wait for child process")
        }
        if (WIFEXITED(Status) || WIFSIGNALED(Status) || WIFSTOPPED(Status))
            CREATE_RESULT(true, "Child process exited")
    }
}
RESULT SUBMISSION::CreateFiles()
{
    std::ofstream LogFile(WorkDir + "/main.log");
    if (!LogFile.is_open())
        CREATE_RESULT(false, "Can not open data file")
    LogFile.close();

    if (chown((WorkDir + "/main.log").c_str(), Settings.GetJudgeUserID(), Settings.GetJudgeUserGroupID()) == -1)
        CREATE_RESULT(false, "Can not change group of compile log file")

    if (chmod((WorkDir + "/main.log").c_str(), 0760) == -1)
        CREATE_RESULT(false, "Can not change permission of compile log file")

    std::ofstream OutputFile(WorkDir + "/main");
    if (!OutputFile.is_open())
        CREATE_RESULT(false, "Can not open data file")
    OutputFile.close();

    if (chown((WorkDir + "/main").c_str(), Settings.GetJudgeUserID(), Settings.GetJudgeUserGroupID()) == -1)
        CREATE_RESULT(false, "Can not change group of compile output file")

    if (chmod((WorkDir + "/main").c_str(), 0760) == -1)
        CREATE_RESULT(false, "Can not change permission of compile output file")

    CREATE_RESULT(true, "Create files")
}
RESULT SUBMISSION::Compile()
{
    UpdateAllResults(JUDGE_RESULT::COMPILING);

    RETURN_IF_FAILED(CreateFiles())

    pid_t PID = fork();
    if (PID == 0)
    {
        RETURN_IF_FAILED(ChildProcess())
        exit(0);
    }
    else
    {
        this->ProcessID = PID;
        RETURN_IF_FAILED(ParentProcess())
        RETURN_IF_FAILED(RemoveEnvrionment())

        struct stat OutputFileStatus;

        if (stat((WorkDir + "/main").c_str(), &OutputFileStatus) < 0)
            CREATE_RESULT(false, "Can not open check compile output file status")

        if (OutputFileStatus.st_size != 0)
        {
            UpdateAllResults(JUDGE_RESULT::COMPILED);
            CREATE_RESULT(true, "Submission compiled successfully")
        }

        std::ifstream CompileLog(WorkDir + "/main.log");
        if (!CompileLog.is_open())
            CREATE_RESULT(false, "Can not open compile output log file")

        std::string CompileResult;
        std::string Line;
        while (getline(CompileLog, Line))
            CompileResult += Line + "\n";
        CompileLog.close();

        if (CompileResult == "")
            CREATE_RESULT(false, "Compiler output log file is empty but compile failed")
        UpdateAllResults(JUDGE_RESULT::COMPILATION_ERROR);
        Description = CompileResult;
        CREATE_RESULT(true, "Submission compile failed")
    }

    CREATE_RESULT(false, "Should't go here");
}
RESULT SUBMISSION::RunTestGroups()
{
    if (Result != JUDGE_RESULT::COMPILED)
        CREATE_RESULT(true, "Submission is not compiled")
    if (Problem.IOFilename == "")
        Problem.IOFilename = std::to_string(SID);
    UpdateAllResults(JUDGE_RESULT::JUDGING);
    for (size_t i = 0; i < TestGroups.size(); i++)
    {
        RETURN_IF_FAILED(TestGroups[i].Judge())
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
        if (ResultCount[i] > MaxCount)
        {
            SecondMaxCount = MaxCount;
            SecondMaxResult = MaxResult;
            MaxCount = ResultCount[i];
            MaxResult = (JUDGE_RESULT)i;
        }
        else if (ResultCount[i] > SecondMaxCount)
        {
            SecondMaxCount = ResultCount[i];
            SecondMaxResult = (JUDGE_RESULT)i;
        }
    Result = (SecondMaxCount == 0 || MaxResult != JUDGE_RESULT::ACCEPTED) ? MaxResult : SecondMaxResult;

    CREATE_RESULT(true, "Submission run test groups")
}

RESULT SUBMISSION::Set(std::string Code, std::string PID)
{
    UpdateAllResults(JUDGE_RESULT::WAITING);
    this->Code = Code;
    this->PID = PID;
    if (Code.length() > 1024 * 1024)
        CREATE_RESULT(false, "Source code is too long")
    CREATE_RESULT(true, "Set submission successfully")
}

RESULT SUBMISSION::Judge()
{
    if (Result != JUDGE_RESULT::WAITING)
        CREATE_RESULT(false, "Submission has been judged")

    bool UpdateDatabaseSignal = true;
    std::thread UpdateDatabase(
        [this, &UpdateDatabaseSignal]()
        {
            while (UpdateDatabaseSignal)
            {
                SUBMISSIONS::UpdateSubmission(this);
                sleep(1);
            }
        });

    UpdateAllResults(JUDGE_RESULT::FETCHED);

    WorkDir = Settings.GetRunDir() + "/" + std::to_string(SID);
    RETURN_IF_FAILED(UTILITIES::MakeDir(WorkDir))

    std::ofstream SourceFile = std::ofstream(WorkDir + "/main.cpp");
    if (!SourceFile.is_open())
        CREATE_RESULT(false, "Can not open source file to write")
    SourceFile << Code;
    SourceFile.close();

    RESULT CompileResult = Compile();
    if (!CompileResult.Success)
        UpdateAllResults(JUDGE_RESULT::SYSTEM_ERROR);

    RESULT RunTestGroupsResult = RunTestGroups();
    if (!RunTestGroupsResult.Success)
    {
        UpdateAllResults(JUDGE_RESULT::SYSTEM_ERROR);
        Description = RunTestGroupsResult.Message;
        RETURN_IF_FAILED(RunTestGroupsResult)
    }

    UpdateDatabaseSignal = false;
    UpdateDatabase.join();
    RETURN_IF_FAILED(UTILITIES::RemoveDir(WorkDir));
    SUBMISSIONS::UpdateSubmission(this);

    CREATE_RESULT(true, "Judge submission successfully")
}
