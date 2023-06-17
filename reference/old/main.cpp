#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#define SE                                                                                   \
    {                                                                                        \
        for (unsigned int i = 0; i < TestPoints.size(); i++)                                 \
        {                                                                                    \
            TestPoints[i].ErrorMessage = "System error, error code: " + to_string(__LINE__); \
            TestPoints[i].Status = "SE";                                                     \
        }                                                                                    \
        Status = (Status == 3 ? 4 : 3);                                                      \
        return;                                                                              \
    }
using namespace std;

string CurrentDir;
class JUDGE
{
private:
    int TempExitNumberTransfer = 0;
    int Status = 0;
    void __Compile();
    static void *_Compile(void *This);
    void __Judge();
    static void *_Judge(void *This);
    string StringReplaceAll(string Input, string Before, string After);

public:
    string MySystem(string Command, FILE *InputRedirect = NULL, FILE *OutputRedirect = NULL, int *Status = NULL);
    struct TEST_POINT
    {
        string StanderInput = "";
        string StanderOutput = "";
        string UserOutput = "";
        int MemoryLimit = 128 * 1024;
        int MemoryUsed = 0;
        int TimeLimit = 1000;
        int TimeUsed = 0;
        string Status = "UKE";
        string ErrorMessage = "";
    };
    int Score = 0;
    string SourceCode = "";
    string IOFilename = "";
    string JudgeFolderName = "";
    string StanderInputFileName = "Answer.in";
    string UserOutputFileName = "Answer.out";
    string SourceCodeName = "main.cpp";
    string ExecutableFileName = "main";
    bool Compiled = false;
    bool RunFinished = false;
    bool Accepted = false;
    vector<TEST_POINT> TestPoints;
    void Init();
    void Compile();
    void Judge();
    void Clean();
};
string JUDGE::MySystem(string Command, FILE *InputRedirect, FILE *OutputRedirect, int *Status)
{
    Command += " ";
    int __fd[2];
    FILE *__stream;
    pid_t pid;
    if (pipe(__fd) != 0)
        return "ERR1";
    if ((pid = vfork()) < 0)
        return "ERR2";
    else if (pid == 0)
    {
        queue<string> TempArguments;
        int FileNameStartPos = Command.find(" ");
        string FileName = Command.substr(0, FileNameStartPos);
        TempArguments.push(FileName);
        Command = Command.substr(FileNameStartPos + 1, string::npos);
        while (Command != "")
        {
            int ArgumentStartPos = Command.find(" ");
            TempArguments.push(Command.substr(0, ArgumentStartPos));
            Command = Command.substr(ArgumentStartPos + 1, string::npos);
        }
        char **Arguments = new char *[TempArguments.size() + 1];
        int Counter = 0;
        while (!TempArguments.empty())
        {
            Arguments[Counter] = new char[TempArguments.front().size() + 1];
            strcpy(Arguments[Counter], TempArguments.front().c_str());
            TempArguments.pop();
            Counter++;
        }
        Arguments[Counter] = NULL;
        if (InputRedirect != NULL)
            dup2(fileno(InputRedirect), STDIN_FILENO);
        close(__fd[0]);
        if (OutputRedirect == NULL)
        {
            dup2(__fd[1], STDOUT_FILENO);
            dup2(__fd[1], STDERR_FILENO);
        }
        else
        {
            dup2(fileno(OutputRedirect), STDOUT_FILENO);
            dup2(fileno(OutputRedirect), STDERR_FILENO);
        }
        close(__fd[1]);
        execvp(FileName.c_str(), Arguments);
        cout << "FAILED " << errno << endl;
        delete Arguments;
        _exit(127);
    }
    close(__fd[1]);
    if (Status != NULL)
        waitpid(pid, Status, 0);
    string Output = "";
    if ((__stream = fdopen(__fd[0], "r")) == NULL)
        return "ERR3";
    while (!feof(__stream))
        Output.push_back(fgetc(__stream));
    fclose(__stream);
    Output.erase(Output.size() - 1, 1);
    return Output;
}
string JUDGE::StringReplaceAll(string Input, string Before, string After)
{
    long unsigned int FoundedPos = Input.find(Before);
    while (FoundedPos != string::npos)
    {
        Input.replace(FoundedPos, Before.size(), After);
        FoundedPos = Input.find(Before);
    }
    return Input;
}
void JUDGE::Init()
{
    if (Status != 0)
        SE;
    struct timeval TimeVal;
    struct timezone TimeZone;
    if (gettimeofday(&TimeVal, &TimeZone) != 0)
        SE;
    JudgeFolderName = to_string(TimeVal.tv_sec * 1000000 + TimeVal.tv_usec);
    if (mkdir(JudgeFolderName.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0)
        SE;
    if (chdir(JudgeFolderName.c_str()) != 0)
        SE;
    FILE *SourceOutputFilePointer = fopen(SourceCodeName.c_str(), "w");
    if (SourceOutputFilePointer == NULL)
        SE;
    if (fprintf(SourceOutputFilePointer, "%s", SourceCode.c_str()) < 0)
        SE;
    if (fclose(SourceOutputFilePointer) != 0)
        SE;
    if (IOFilename != "")
    {
        StanderInputFileName = IOFilename + ".in";
        UserOutputFileName = IOFilename + ".out";
    }
    Status = 1;
}
void *JUDGE::_Compile(void *This)
{
    JUDGE *CurrentClass = (JUDGE *)This;
    CurrentClass->__Compile();
    return NULL;
}
void JUDGE::__Compile()
{
    MySystem(string("gcc " + SourceCodeName + " -O2 -std=c++14 -lstdc++ -lm -DONLINE_JUDGE -o " + ExecutableFileName));
    Compiled = true;
    sleep(INT32_MAX);
}
void JUDGE::Compile()
{
    if (Status != 1)
        return;
    pthread_t CompileThread;
    if (pthread_create(&CompileThread, NULL, _Compile, (void *)this) != 0)
        SE;
    time_t Now = time(NULL);
    while (time(NULL) - Now < 10 && !Compiled)
        ;
    if (pthread_cancel(CompileThread) != 0)
        SE;
    if (pthread_join(CompileThread, NULL) != 0)
        SE;
    if (!Compiled)
    {
        for (unsigned int i = 0; i < TestPoints.size(); i++)
        {
            TestPoints[i].ErrorMessage = "Compile time limit exceeded. ";
            TestPoints[i].Status = "CE";
        }
        Status = 3;
        return;
    }
    struct stat Temp;
    if (stat(ExecutableFileName.c_str(), &Temp) != 0)
    {
        for (unsigned int i = 0; i < TestPoints.size(); i++)
        {
            TestPoints[i].ErrorMessage = "Compile failed. ";
            TestPoints[i].Status = "CE";
        }
        Status = 3;
        return;
    }
    Status = 2;
}
void *JUDGE::_Judge(void *This)
{
    JUDGE *CurrentClass = (JUDGE *)This;
    CurrentClass->__Judge();
    return NULL;
}
void JUDGE::__Judge()
{
    FILE *InputFileRedirect = fopen(string(CurrentDir + JudgeFolderName + "/" + StanderInputFileName).c_str(), "r");
    if (InputFileRedirect == NULL)
        return;
    FILE *OutputFileRedirect = fopen(string(CurrentDir + JudgeFolderName + "/" + UserOutputFileName).c_str(), "w");
    if (OutputFileRedirect == NULL)
        return;
    int ExitNumber = 0;
    MySystem(CurrentDir + JudgeFolderName + "/" + ExecutableFileName, InputFileRedirect, OutputFileRedirect, &ExitNumber);
    fclose(InputFileRedirect);
    fclose(OutputFileRedirect);
    TempExitNumberTransfer = ExitNumber;
    RunFinished = true;
    sleep(INT32_MAX);
}
void JUDGE::Judge()
{
    if (Status != 2)
        return;
    for (unsigned int i = 0; i < TestPoints.size(); i++)
    {
        FILE *StanderInputFilePointer = fopen(StanderInputFileName.c_str(), "w");
        if (StanderInputFilePointer == NULL)
            SE;
        if (fprintf(StanderInputFilePointer, "%s", TestPoints[i].StanderInput.c_str()) < 0)
            SE;
        if (fclose(StanderInputFilePointer) != 0)
            SE;

        rlimit Limit;
        Limit.rlim_cur = TestPoints[i].MemoryLimit;
        Limit.rlim_max = TestPoints[i].MemoryLimit;
        if (setrlimit(RLIMIT_AS, &Limit) != 0)
            SE;

        Limit.rlim_cur = TestPoints[i].TimeLimit / CLOCKS_PER_SEC;
        Limit.rlim_max = Limit.rlim_cur + 1;
        if (setrlimit(RLIMIT_CPU, &Limit) != 0)
            SE;

        Limit.rlim_cur = 0;
        Limit.rlim_max = 0;
        if (setrlimit(RLIMIT_CORE, &Limit) != 0)
            SE;

        RunFinished = false;
        pthread_t JudgeThread;
        if (pthread_create(&JudgeThread, NULL, _Judge, (void *)this) != 0)
            SE;
        clock_t Now = clock();
        while (clock() - Now < TestPoints[i].TimeLimit && !RunFinished)
            ;
        TestPoints[i].TimeUsed = clock() - Now;
        if (pthread_cancel(JudgeThread) != 0)
            SE;
        if (pthread_join(JudgeThread, NULL) != 0)
            SE;
        if (!RunFinished)
        {
            TestPoints[i].Status = "TLE";
            TestPoints[i].ErrorMessage = "Time limit exceeded. ";
            continue;
        }
        FILE *UserOutputFilePointer = fopen(UserOutputFileName.c_str(), "r");
        if (UserOutputFilePointer == NULL)
            SE;
        while (!feof(UserOutputFilePointer))
            TestPoints[i].UserOutput.push_back(fgetc(UserOutputFilePointer));
        fclose(UserOutputFilePointer);
        TestPoints[i].UserOutput.erase(TestPoints[i].UserOutput.size() - 1, 1);
        TestPoints[i].UserOutput = StringReplaceAll(StringReplaceAll(TestPoints[i].UserOutput, " \n", "\n"), "\n\n", "\n");
        while (TestPoints[i].UserOutput.size() > 0 && TestPoints[i].UserOutput[TestPoints[i].UserOutput.size() - 1] == '\n')
            TestPoints[i].UserOutput.erase(TestPoints[i].UserOutput.size() - 1, 1);
        if (WIFEXITED(TempExitNumberTransfer) && WEXITSTATUS(TempExitNumberTransfer) != 0)
        {
            TestPoints[i].Status = "RTE";
            TestPoints[i].ErrorMessage = string("Run time error, Program has exited with code " + to_string(WEXITSTATUS(TempExitNumberTransfer)) + ". ");
        }
        else if (WIFSIGNALED(TempExitNumberTransfer))
        {
            TestPoints[i].Status = "RTE";
            TestPoints[i].ErrorMessage = string("Run time error, Program has exited with signal " + to_string(WTERMSIG(TempExitNumberTransfer)) + ". ");
        }
        else if (WIFSTOPPED(TempExitNumberTransfer))
        {
            TestPoints[i].Status = "RTE";
            TestPoints[i].ErrorMessage = string("Run time error, Program has stopped with code " + to_string(WSTOPSIG(TempExitNumberTransfer)) + ". ");
        }
        else if (TestPoints[i].UserOutput == TestPoints[i].StanderOutput)
        {
            TestPoints[i].Status = "AC";
            TestPoints[i].ErrorMessage = "Accepted. ";
        }
        else
        {
            TestPoints[i].Status = "WA";
            TestPoints[i].ErrorMessage = "Wrong answer. ";
        }
    }
    Status = 3;
}
void JUDGE::Clean()
{
    if (Status != 3)
        return;
    DIR *DirPointer = opendir(".");
    dirent *DirentPointer = NULL;
    while ((DirentPointer = readdir(DirPointer)) != NULL)
        if (DirentPointer->d_type == DT_REG)
            remove(DirentPointer->d_name);
    closedir(DirPointer);
    if (chdir("..") != 0)
        SE;
    if (rmdir(JudgeFolderName.c_str()) != 0)
        SE;
    Status = 4;
}
int main()
{
    JUDGE JudgeStatus;

    JudgeStatus.SourceCode = R"(#include <bits/stdc++.h>
    using namespace std;
    int main()
    {
        freopen("output.in", "r", stdin);
        freopen("output.out", "w", stdout);
        int a;
        cin >> a;
        if (a == 1)
            cout << 1 << endl;
        else if (a == 2)
            cout << "2    " << endl << "    " << endl << endl;
        else if (a == 3)
            cout << "2" << endl;
        else if (a == 4)
            return 1;
        else if (a == 5)
            while (true);
        else if (a == 6)
            throw("ERR");
        return 0;
    })";

    JudgeStatus.IOFilename = "output";
    JUDGE::TEST_POINT Temp;
    Temp.TimeLimit = 1 * CLOCKS_PER_SEC;
    Temp.StanderInput = Temp.StanderOutput = "1";
    JudgeStatus.TestPoints.push_back(Temp);
    Temp.StanderInput = Temp.StanderOutput = "2";
    JudgeStatus.TestPoints.push_back(Temp);
    Temp.StanderInput = Temp.StanderOutput = "3";
    JudgeStatus.TestPoints.push_back(Temp);
    Temp.StanderInput = Temp.StanderOutput = "4";
    JudgeStatus.TestPoints.push_back(Temp);
    Temp.StanderInput = Temp.StanderOutput = "5";
    JudgeStatus.TestPoints.push_back(Temp);
    Temp.StanderInput = Temp.StanderOutput = "6";
    JudgeStatus.TestPoints.push_back(Temp);
    JudgeStatus.Init();
    JudgeStatus.Compile();
    JudgeStatus.Judge();
    JudgeStatus.Clean();
    for (unsigned int i = 0; i < JudgeStatus.TestPoints.size(); i++)
        cout << JudgeStatus.TestPoints[i].Status << " " << JudgeStatus.TestPoints[i].MemoryUsed << "MB " << JudgeStatus.TestPoints[i].TimeUsed * 1.0 / CLOCKS_PER_SEC << "s " << JudgeStatus.TestPoints[i].ErrorMessage << endl;
    return 0;
}
