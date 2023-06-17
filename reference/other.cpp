#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>
#include <string>
#include "constant.h"
extern "C"
{
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>
}
int syscallTable[256];
const std::string syscallNames[256];
int Execute(const int lang, const std::string inputFile, const std::string outputFile, const std::string execFile, const int memoryLimit, const int timeLimit, const int outputLimit, int &execMemory, double &execTime);
bool IsLeagalSyscall(int id)
{
    if (syscallTable[id] < 0)
        return true;
    else if (syscallTable[id] > 0)
    {
        syscallTable[id]--;
        return true;
    }
    else
        return false;
}
void Initsyscall(int lang)
{
    memset(syscallTable, 0, sizeof(syscallTable));
    if (lang == langC || lang == langCpp || lang == langCpp11)
    {
        syscallTable[0] = -1;
        syscallTable[1] = -1;
        syscallTable[5] = -1;
        syscallTable[9] = -1;
        syscallTable[12] = -1;
        syscallTable[21] = -1;
        syscallTable[59] = 1;
        syscallTable[63] = 1;
        syscallTable[89] = 1;
        syscallTable[158] = 1;
    }
    if (lang == langJava)
    { /*...*/
    }
    if (lang == langCs)
    { /*...*/
    }
    if (lang == langPython)
    { /*...*/
    }
    if (lang == langPas)
    { /*...*/
    }
    // 太多了省略掉啦
}

int Execute(const int lang, const std::string inputFile, const std::string outputFile, const std::string execFile,
            const int memoryLimit, const int timeLimit, const int outputLimit, int &execMemory, double &execTime)
{
    execTime = 0;
    rlimit limit;
    pid_t exe = fork();
    if (exe < 0)
    {
        printf("[Exec-Parent]:  fork error.\n");
        exit(1);
    }
    if (exe == 0)
    {
        printf("[Exec-child]:  ready.\n");
        // 时间的限制
        limit.rlim_cur = timeLimit;
        limit.rlim_max = timeLimit + 1;
        if (setrlimit(RLIMIT_CPU, &limit))
        {
            printf("[Exec-child]:  set time limit error.\n");
            exit(1);
        }
        // 输出文件大小的限制
        limit.rlim_cur = outputLimit - 1;
        limit.rlim_max = outputLimit;
        if (setrlimit(RLIMIT_FSIZE, &limit))
        {
            printf("[Exec-child]:  set output limit error.\n");
            exit(1);
        }
        // 转存文件大小的限制
        limit.rlim_cur = 0;
        limit.rlim_max = 0;
        if (setrlimit(RLIMIT_CORE, &limit))
        {
            printf("[Exec-child]:  set core limit error.\n");
            exit(1);
        }
        int out = open(outputFile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
        int in = open(inputFile.c_str(), O_RDWR, 0666);
        dup2(out, STDOUT_FILENO);
        dup2(in, STDIN_FILENO);
        close(in);
        close(out);
        if (lang == langPython || lang == langJava)
        {
            int err = open("run.log", O_CREAT | O_TRUNC | O_RDWR, 0666);
            dup2(err, STDERR_FILENO);
            close(err);
        }
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        switch (lang)
        {
        case langJava:
            execlp("java", "java", "-Djava.security.manager", "-Djava.security.policy=java.policy", "Main", NULL);
            break;
        case langPython:
            execlp("python", "python", execFile.c_str(), NULL);
            break;
        case langCs:
            execlp("mono", "mono", execFile.c_str(), NULL);
            break;
        default:
            execl(execFile.c_str(), execFile.c_str(), NULL);
        }
        fprintf(stderr, "[Exec-child]:  execute failed.\n");
        exit(1);
    }
    printf("[Exec-parent]:  child process id:%d.\n", exe);
    bool syscallEnter = true;
    int status, exitCode, sig, syscallId, syscallCount = 0, maxMem = 0, nowMem = 0;
    rusage ru;
    user_regs_struct regs;
    Initsyscall(lang);
    ptrace(PTRACE_SYSCALL, exe, 0, 0);
    while (wait4(exe, &status, 0, &ru) > 0) // wait4函数等待子进程状态改变
    {
        if (WIFEXITED(status)) // 子进程退出
        {
            printf("[Exec-parent]:  %d exited; status = %d \n", exe, WEXITSTATUS(status));
            if (WEXITSTATUS(status) == EXIT_SUCCESS)
                exitCode = executeSucceed;
            else
                exitCode = executeRuntimeError;
            break;
        }
        if (WIFSIGNALED(status) || (WIFSTOPPED(status) && WSTOPSIG(status) != SIGTRAP)) // 接受到了特定信号
        {
            if (WIFSIGNALED(status))
                sig = WTERMSIG(status);
            else
                sig = WSTOPSIG(status);
            switch (sig)
            {
            case SIGXCPU:
                exitCode = executeTimeLimitExceeded;
                break;
            case SIGXFSZ:
                exitCode = executeOutputLimitExceeded;
                break;
            case SIGSEGV:
            case SIGFPE:
            case SIGBUS:
            case SIGABRT:
            default:
                exitCode = executeRuntimeError;
                break;
            }
            ptrace(PTRACE_KILL, exe, 0, 0);
            break;
        }
        ptrace(PTRACE_GETREGS, exe, 0, &regs);
        syscallId = regs.orig_rax;
        if (syscallEnter)

        {
            syscallCount++;
            printf("\n[Exec-parent]:  <trace>----entering syscall----\n");
            printf("[Exec-parent]:  <trace> : syscalls[%d]: %s\n",
                   syscallId, syscallNames[syscallId].c_str());
            if (syscallId == 12)

            {
                printf("[Exec-parent]:  <trace> : brk (ebx = 0x%08llx) %llu\n", regs.rbx, regs.rbx);
            }
            if (!IsLeagalSyscall(syscallId))

            {
                printf("[Exec-parent]:  <trace> : syscalls[%d]: %s : Restrict function!\n", syscallId,
                       syscallNames[syscallId].c_str());
                printf("[Exec-parent]:  <trace> : killing process %d.\n", exe);
                ptrace(PTRACE_KILL, exe, 0, 0);
                exitCode = executeRestrictFunction;
                break;
            }
        }
        else
        {
            int m = getpagesize() * ru.ru_minflt;
            if (m != nowMem)

            {
                printf("[Exec-parent]:  proc %d memory usage: %dk\n", exe, m);
                nowMem = m;
                maxMem = nowMem > maxMem ? nowMem : maxMem;
                if (nowMem > memoryLimit)

                {
                    printf("[Exec-parent]:  Memory Limit Exceed\n");
                    printf("[Exec-parent]:  killing process %d.\n", exe);
                    ptrace(PTRACE_KILL, exe, 0, 0);
                    exitCode = executeMemoryLimitExceeded;
                    continue;
                }
            }
            printf("[Exec-parent]:  <trace>----leaving syscall----\n\n");
        }
        syscallEnter = !syscallEnter;
        ptrace(PTRACE_SYSCALL, exe, 0, 0);
    }
    printf("[Exec-parent]:  maximum memory used by %s: %dk\n", execFile.c_str(), maxMem);
    printf("[Exec-parent]:  utime sec %d, usec %06d\n", (int)ru.ru_utime.tv_sec, (int)ru.ru_utime.tv_usec);
    printf("[Exec-parent]:  stime sec %d, usec %06d\n", (int)ru.ru_stime.tv_sec, (int)ru.ru_stime.tv_usec);
    printf("[Exec-parent]:  mem usage %d \n", (int)ru.ru_maxrss);
    execTime = ru.ru_utime.tv_sec + ru.ru_utime.tv_usec * 1e-6 + ru.ru_stime.tv_sec + ru.ru_stime.tv_usec * 1e-6;
    execMemory = nowMem;
    printf("[Exec-parent]:  cputime used %.4lf\n", execTime);
    printf("[Exec-parent]:  exiting total syscall is %d\n", syscallCount);
    return exitCode;
}
int main()
{
    int execMemory;
    double execTime;
    Execute(langCpp, "test.in", "test.out", "test", 1024 * 1024, 1000, 1000, execMemory, execTime);
}
