#include "Settings.hpp"
#include "Utilities.hpp"
#include "Database.hpp"
#include <unistd.h>
#include <fstream>
#include <vector>
#include <sys/stat.h>

RESULT SETTINGS::Save()
{
    CheckSettings();
    std::string SystemCallsString = "";
    for (int i = 0; i < SystemCallCount; i++)
        SystemCallsString += SystemCalls[i].Name + " " + std::to_string(SystemCalls[i].AvailableCount) + "\n";
    RETURN_IF_FAILED(UTILITIES::SaveFile(SettingBaseFolder + "/JudgeUser", JudgeUser))
    RETURN_IF_FAILED(UTILITIES::SaveFile(SettingBaseFolder + "/Compiler", Compiler))
    RETURN_IF_FAILED(UTILITIES::SaveFile(SettingBaseFolder + "/SocketPort", std::to_string(SocketPort)))
    RETURN_IF_FAILED(UTILITIES::SaveFile(SettingBaseFolder + "/CompileTimeLimit", std::to_string(CompileTimeLimit)))
    RETURN_IF_FAILED(UTILITIES::SaveFile(SettingBaseFolder + "/CompileOutputLimit", std::to_string(CompileOutputLimit)))
    RETURN_IF_FAILED(UTILITIES::SaveFile(SettingBaseFolder + "/Email", Email))
    RETURN_IF_FAILED(UTILITIES::SaveFile(SettingBaseFolder + "/EmailPassword", EmailPassword))
    RETURN_IF_FAILED(UTILITIES::SaveFile(SettingBaseFolder + "/SystemCalls", SystemCallsString))
    CREATE_RESULT(true, "Settings saved")
}
RESULT SETTINGS::Load(std::string JudgeUser)
{
    std::string SettingBaseFolder = "/home/" + JudgeUser + "/Settings";
    std::string SystemCallsString;
    RETURN_IF_FAILED(UTILITIES::LoadFile(SettingBaseFolder + "/JudgeUser", JudgeUser))
    RETURN_IF_FAILED(UTILITIES::LoadFile(SettingBaseFolder + "/Compiler", Compiler))
    RETURN_IF_FAILED(UTILITIES::LoadFile(SettingBaseFolder + "/SocketPort", SocketPort))
    RETURN_IF_FAILED(UTILITIES::LoadFile(SettingBaseFolder + "/CompileTimeLimit", CompileTimeLimit))
    RETURN_IF_FAILED(UTILITIES::LoadFile(SettingBaseFolder + "/CompileOutputLimit", CompileOutputLimit))
    RETURN_IF_FAILED(UTILITIES::LoadFile(SettingBaseFolder + "/Email", Email))
    RETURN_IF_FAILED(UTILITIES::LoadFile(SettingBaseFolder + "/EmailPassword", EmailPassword))
    RETURN_IF_FAILED(UTILITIES::LoadFile(SettingBaseFolder + "/SystemCalls", SystemCallsString))
    std::vector<std::string> SystemCallsLines;
    for (size_t i = 0; i < SystemCallsString.size(); i++)
        if (SystemCallsString[i] == '\n')
        {
            SystemCallsLines.push_back(SystemCallsString.substr(0, i));
            SystemCallsString = SystemCallsString.substr(i + 1);
            i = 0;
        }
    for (size_t i = 0; i < SystemCallsLines.size(); i++)
    {
        std::string SystemCallName = SystemCallsLines[i].substr(0, SystemCallsLines[i].find(' '));
        int SystemCallAvailableCount = std::stoi(SystemCallsLines[i].substr(SystemCallsLines[i].find(' ') + 1));
        SystemCalls[i].Name = SystemCallName;
        SystemCalls[i].AvailableCount = SystemCallAvailableCount;
    }
    CheckSettings();
    CREATE_RESULT(true, "Settings loaded")
}

std::string SETTINGS::GetBaseFolder() { return BaseFolder; }
std::string SETTINGS::GetSettingsBaseFolder() { return SettingBaseFolder; }
std::string SETTINGS::GetJudgeUser() { return JudgeUser; }
int SETTINGS::GetJudgeUserID() { return JudgeUserID; }
// std::string SETTINGS::GetJudgeUserGroup() { return JudgeUserGroup; }
int SETTINGS::GetJudgeUserGroupID() { return JudgeUserGroupID; }
bool SETTINGS::IsBannedSystemCall(int SystemCall, int CallCount)
{
    if (SystemCall < 0 || SystemCall >= SystemCallCount)
    {
        Logger.Warning("System call " + std::to_string(SystemCall) + " is not valid");
        return true;
    }
    return SystemCalls[SystemCall].AvailableCount < CallCount &&
           SystemCalls[SystemCall].AvailableCount != -1;
}
std::string SETTINGS::GetSystemCallName(int SystemCall)
{
    if (SystemCall < 0 || SystemCall >= SystemCallCount)
    {
        Logger.Warning("System call " + std::to_string(SystemCall) + " is not valid");
        return "";
    }
    return SystemCalls[SystemCall].Name;
}
std::string SETTINGS::GetCompiler() { return Compiler; }
std::string SETTINGS::GetRunDir() { return RunDir; }
int SETTINGS::GetSocketPort() { return SocketPort; }
int SETTINGS::GetCompileTimeLimit() { return CompileTimeLimit; }
int SETTINGS::GetCompileOutputLimit() { return CompileOutputLimit; }
std::string SETTINGS::GetEmail() { return Email; }
std::string SETTINGS::GetEmailPassword() { return EmailPassword; }

std::string SETTINGS::DATABASE_SETTINGS::GetHost() { return Host; }
int SETTINGS::DATABASE_SETTINGS::GetPort() { return Port; }
std::string SETTINGS::DATABASE_SETTINGS::GetUsername() { return Username; }
std::string SETTINGS::DATABASE_SETTINGS::GetPassword() { return Password; }
std::string SETTINGS::DATABASE_SETTINGS::GetDatabaseName() { return DatabaseName; }
RESULT SETTINGS::DATABASE_SETTINGS::Set(std::string Host, int Port, std::string Username, std::string Password, std::string DatabaseName)
{
    this->Host = Host;
    this->Port = Port;
    this->Username = Username;
    this->Password = Password;
    this->DatabaseName = DatabaseName;
    RETURN_IF_FALSE(DATABASE::CreateConnection() == nullptr, "Database settings are not valid")
    CREATE_RESULT(true, "Database settings set")
}

void SETTINGS::SetJudgeUser(std::string JudgeUser)
{
    this->JudgeUser = JudgeUser;
    CheckJudgeUser();
}
void SETTINGS::SetSystemCallAvailableCount(int SystemCall, int AvailableCount)
{
    if (SystemCall < 0 || SystemCall >= SystemCallCount)
        Logger.Warning("System call " + std::to_string(SystemCall) + " is not valid");
    else
    {
        SystemCalls[SystemCall].AvailableCount = AvailableCount;
    }
}
void SETTINGS::SetCompiler(std::string Compiler)
{
    this->Compiler = Compiler;
    CheckCompiler();
}
void SETTINGS::SetSocketPort(int SocketPort) { this->SocketPort = SocketPort; }
void SETTINGS::SetCompileTimeLimit(int CompileTimeLimit) { this->CompileTimeLimit = CompileTimeLimit; }
void SETTINGS::SetCompileOutputLimit(int CompileOutputLimit) { this->CompileOutputLimit = CompileOutputLimit; }
void SETTINGS::SetEmail(std::string Email) { this->Email = Email; }
void SETTINGS::SetEmailPassword(std::string EmailPassword) { this->EmailPassword = EmailPassword; }

void SETTINGS::CheckJudgeUser()
{
    if (JudgeUser == "")
        Logger.Fetal("Judge user is empty");
    GetJudgeUserIDByUserName();

    BaseFolder = "/home/" + JudgeUser;
    if (access(BaseFolder.c_str(), F_OK) == -1)
    {
        if (mkdir(BaseFolder.c_str(), 0755) == -1)
            Logger.Fetal("Can not create base folder");
        if (chown(BaseFolder.c_str(), JudgeUserID, JudgeUserGroupID) == -1)
            Logger.Fetal("Can not change base folder owner");
    }

    SettingBaseFolder = BaseFolder + "/Settings";
    if (access(SettingBaseFolder.c_str(), F_OK) == -1)
    {
        if (mkdir(SettingBaseFolder.c_str(), 0755) == -1)
            Logger.Fetal("Can not create setting base folder");
        if (chown(SettingBaseFolder.c_str(), JudgeUserID, JudgeUserGroupID) == -1)
            Logger.Fetal("Can not change setting base folder owner");
    }

    RunDir = BaseFolder + "/Run";
    if (access(RunDir.c_str(), F_OK) == -1)
    {
        if (mkdir(RunDir.c_str(), 0755) == -1)
            Logger.Fetal("Can not create run folder");
        if (chown(RunDir.c_str(), JudgeUserID, JudgeUserGroupID) == -1)
            Logger.Fetal("Can not change run folder owner");
    }
}
void SETTINGS::GetJudgeUserIDByUserName()
{
    std::ifstream InputFile("/etc/passwd");
    if (!InputFile.is_open())
        Logger.Fetal("Can not open password file \"/etc/passwd\"");
    std::string Line;
    while (getline(InputFile, Line))
    {
        if (Line.substr(0, Line.find(":")) == JudgeUser)
        {
            int StartPosition = Line.find(":", Line.find(":") + 1) + 1;
            int EndPosition = Line.find(":", StartPosition);
            JudgeUserID = atoi(Line.substr(StartPosition, EndPosition - StartPosition).c_str());
            StartPosition = EndPosition;
            EndPosition = Line.find(":", StartPosition);
            JudgeUserGroupID = atoi(Line.substr(StartPosition, EndPosition - StartPosition).c_str());
            // GetJudgeUserGroupNameByGroupID();
            return;
        }
    }
    Logger.Fetal("Judge user \"" + JudgeUser + "\" does not exist");
}
// void SETTINGS::GetJudgeUserGroupNameByGroupID()
// {
//     std::ifstream InputFile("/etc/group");
//     std::string Line;
//     while (getline(InputFile, Line))
//     {
//         int FirstColonPosition = Line.find(":");
//         int SecondColonPosition = Line.find(":", FirstColonPosition);
//         int ThirdColonPosition = Line.find(":", SecondColonPosition);
//         if (Line.substr(SecondColonPosition, ThirdColonPosition - SecondColonPosition) == std::to_string(JudgeUserGroupID))
//         {
//             JudgeUserGroup = Line.substr(0, FirstColonPosition);
//             //             return;
//         }
//     }
//     Logger.Fetal("Judge user group " + std::to_string(JudgeUserGroupID) + " does not exist");
// }
void SETTINGS::CheckCompiler()
{
    if (access(Compiler.c_str(), X_OK) != 0)
        Logger.Fetal("Compiler does not exist");
}

void SETTINGS::CheckSettings()
{
    CheckJudgeUser();
    CheckCompiler();
}

SETTINGS Settings;
