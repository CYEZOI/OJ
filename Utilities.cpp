#include <sys/stat.h>
#include <sys/dir.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include "Settings.hpp"
#include "Utilities.hpp"

std::string UTILITIES::StringReplaceAll(std::string Data, std::string Search, std::string Replace)
{
    size_t Pos = Data.find(Search);
    while (Pos != std::string::npos)
    {
        Data.replace(Pos, Search.size(), Replace);
        Pos = Data.find(Search, Pos + Replace.size());
    }
    return Data;
}
// RESULT UTILITIES::MakeDir(std::string Dir)
// {
//     if (access(Dir.c_str(), F_OK) != -1)
//     {
//         if (!RemoveDir(Dir))
//         {
//             Logger.Error("Can not remove working directory " + Dir);
//             return false;
//         }
//     }
//     if (mkdir(Dir.c_str(), 0755) == -1)
//     {
//         Logger.Error("Can not recreate working directory " + Dir);
//         return false;
//     }
//     return true;
// }
RESULT UTILITIES::MakeDir(std::string Dir)
{
    if (access(Dir.c_str(), F_OK) == -1)
        CREATE_RESULT_IF_FALSE(mkdir(Dir.c_str(), 0755) == 0, "Can not create working directory " + Dir)
    CREATE_RESULT(true, "Created working directory " + Dir)
}
RESULT UTILITIES::RemoveDir(std::string Dir)
{
    DIR *DirPtr = opendir(Dir.c_str());
    if (DirPtr == nullptr)
        CREATE_RESULT(false, "Can not open directory " + Dir)
    struct dirent *Entry = readdir(DirPtr);
    while (Entry != nullptr)
    {
        if (Entry->d_type == DT_DIR)
        {
            if (strcmp(Entry->d_name, ".") != 0 && strcmp(Entry->d_name, "..") != 0)
                RemoveDir(Dir + "/" + Entry->d_name);
        }
        else
        {
            if (remove((Dir + "/" + Entry->d_name).c_str()) != 0)
                Logger.Error("Can not remove file " + Dir + "/" + Entry->d_name);
        }
        Entry = readdir(DirPtr);
    }
    closedir(DirPtr);
    if (rmdir(Dir.c_str()) != 0)
        CREATE_RESULT(false, "Can not remove directory " + Dir)
    CREATE_RESULT(true, "Removed directory " + Dir)
}
RESULT UTILITIES::CopyFile(std::string Source, std::string Destination)
{
    FILE *SourceFile = fopen(Source.c_str(), "rb");
    if (SourceFile == nullptr)
        CREATE_RESULT(false, "Can not open source file " + Source)
    FILE *DestinationFile = fopen(Destination.c_str(), "wb");
    if (DestinationFile == nullptr)
    {
        fclose(SourceFile);
        CREATE_RESULT(false, "Can not open destination file " + Destination)
    }
    char Buffer[1024];
    size_t ReadSize = fread(Buffer, 1, 1024, SourceFile);
    while (ReadSize > 0)
    {
        if (fwrite(Buffer, 1, ReadSize, DestinationFile) != ReadSize)
        {
            fclose(SourceFile);
            fclose(DestinationFile);
            CREATE_RESULT(false, "Can not write to destination file " + Destination)
        }
        ReadSize = fread(Buffer, 1, 1024, SourceFile);
    }
    fclose(SourceFile);
    fclose(DestinationFile);

    struct stat FileStatus;
    if (lstat(Source.c_str(), &FileStatus) == -1)
        CREATE_RESULT(false, "Can not get source file \"" + Source + "\" attributions")
    if (chmod(Destination.c_str(), FileStatus.st_mode) == -1)
        CREATE_RESULT(false, "Can not set destination file \"" + Destination + "\" attributions")

    CREATE_RESULT(true, "Copied file \"" + Source + "\" to \"" + Destination + "\"")
}
RESULT UTILITIES::CopyDir(std::string Source, std::string Destination)
{
    DIR *DirPtr = opendir(Source.c_str());
    if (DirPtr == nullptr)
        CREATE_RESULT(false, "Can not open directory " + Source)
    struct dirent *Entry = readdir(DirPtr);
    while (Entry != nullptr)
    {
        if (Entry->d_type == DT_DIR)
        {
            if (strcmp(Entry->d_name, ".") != 0 && strcmp(Entry->d_name, "..") != 0)
            {
                struct stat FileStatus;
                if (lstat((Source + "/" + Entry->d_name).c_str(), &FileStatus) == -1)
                {
                    closedir(DirPtr);
                    CREATE_RESULT(false, "Can not get directory " + Source + "/" + Entry->d_name + " attributions")
                }
                if (mkdir((Destination + "/" + Entry->d_name).c_str(), FileStatus.st_mode) == -1)
                {
                    closedir(DirPtr);
                    CREATE_RESULT(false, "Can not create directory " + Destination + "/" + Entry->d_name)
                }
                RETURN_IF_FAILED(CopyDir(Source + "/" + Entry->d_name, Destination + "/" + Entry->d_name))
            }
        }
        else
            RETURN_IF_FAILED_WITH_OPERATION(CopyFile(Source + "/" + Entry->d_name, Destination + "/" + Entry->d_name), closedir(DirPtr))
        Entry = readdir(DirPtr);
    }
    closedir(DirPtr);
    CREATE_RESULT(true, "Copied directory \"" + Source + "\" to \"" + Destination + "\"")
}
RESULT UTILITIES::LoadFile(std::string FileName, std::string &Output)
{
    FILE *File = fopen(FileName.c_str(), "rb");
    if (File == nullptr)
        CREATE_RESULT(false, "Can not open file " + FileName)
    fseek(File, 0, SEEK_END);
    size_t Size = ftell(File);
    rewind(File);
    char *Buffer = new char[Size + 1];
    if (fread(Buffer, 1, Size, File) != Size)
    {
        fclose(File);
        delete[] Buffer;
        CREATE_RESULT(false, "Can not read file " + FileName)
    }
    Buffer[Size] = '\0';
    Output = Buffer;
    delete[] Buffer;
    fclose(File);
    CREATE_RESULT(true, "Loaded file " + FileName)
}
RESULT UTILITIES::LoadFile(std::string FileName, int &Output)
{
    std::string Temp;
    RETURN_IF_FAILED(LoadFile(FileName, Temp))
    Output = atoi(Temp.c_str());
    CREATE_RESULT(true, "Loaded file " + FileName)
}
RESULT UTILITIES::SaveFile(std::string FileName, std::string Data)
{
    FILE *File = fopen(FileName.c_str(), "wb");
    if (File == nullptr)
        CREATE_RESULT(false, "Can not open file " + FileName)
    if (fwrite(Data.c_str(), 1, Data.size(), File) != Data.size())
    {
        fclose(File);
        CREATE_RESULT(false, "Can not write to file " + FileName)
    }
    fclose(File);
    CREATE_RESULT(true, "Saved file " + FileName)
}
RESULT UTILITIES::SaveFile(std::string FileName, int Data)
{
    return SaveFile(FileName, std::to_string(Data));
}
std::string UTILITIES::RemoveSpaces(std::string Input)
{
    while (Input.size() > 0 && (Input[0] == 0 ||
                                Input[0] == '\n' ||
                                Input[0] == '\r' ||
                                Input[0] == '\t' ||
                                Input[0] == ' '))
        Input.erase(0, 1);
    while (Input.size() > 0 && (Input[Input.size() - 1] == 0 ||
                                Input[Input.size() - 1] == '\n' ||
                                Input[Input.size() - 1] == '\r' ||
                                Input[Input.size() - 1] == '\t' ||
                                Input[Input.size() - 1] == ' '))
        Input.erase(Input.size() - 1, 1);
    return Input;
}
size_t UTILITIES::UploadFunction(char *ptr, size_t size, size_t nmemb, void *userp)
{
    std::string *Upload = (std::string *)userp;
    const char *data = Upload->c_str();
    size_t room = size * nmemb;
    if (room < 1 || data == nullptr)
        return 0;
    size_t len = std::min(room, strlen(data));
    memcpy(ptr, data, len);
    Upload->erase(0, len);
    return len;
}
RESULT UTILITIES::SendEmail(std::string To, std::string Subject, std::string Body)
{
    std::string From = UTILITIES::RemoveSpaces(Settings.GetEmail());
    std::string Password = UTILITIES::RemoveSpaces(Settings.GetEmailPassword());
    CURL *Curl = curl_easy_init();
    if (Curl == nullptr)
        CREATE_RESULT(false, "Can not initialize CURL")
    struct curl_slist *Recipients = nullptr;
    Recipients = curl_slist_append(Recipients, To.c_str());
    curl_easy_setopt(Curl, CURLOPT_URL, "smtp://smtp-mail.outlook.com:587");
    curl_easy_setopt(Curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(Curl, CURLOPT_USERNAME, From.c_str());
    curl_easy_setopt(Curl, CURLOPT_PASSWORD, Password.c_str());
    curl_easy_setopt(Curl, CURLOPT_MAIL_FROM, From.c_str());
    curl_easy_setopt(Curl, CURLOPT_MAIL_RCPT, Recipients);
    curl_easy_setopt(Curl, CURLOPT_UPLOAD, true);
    std::string Message = "From: " + From + "\r\nTo: " + To + "\r\nSubject: " + Subject + "\r\n\r\n" + Body + "\r\n";
    curl_easy_setopt(Curl, CURLOPT_READDATA, &Message);
    curl_easy_setopt(Curl, CURLOPT_READFUNCTION, UploadFunction);
    CURLcode Result = curl_easy_perform(Curl);
    if (Result != CURLE_OK)
    {
        curl_slist_free_all(Recipients);
        curl_easy_cleanup(Curl);
        CREATE_RESULT(false, "Can not send email: " + std::string(curl_easy_strerror(Result)))
    }
    curl_slist_free_all(Recipients);
    curl_easy_cleanup(Curl);
    CREATE_RESULT(true, "Sent email to " + To)
}
time_t UTILITIES::StringToTime(std::string String)
{
    struct tm Time;
    memset(&Time, 0, sizeof(Time));
    if (sscanf(String.c_str(), "%d-%d-%d %d:%d:%d", &Time.tm_year, &Time.tm_mon, &Time.tm_mday, &Time.tm_hour, &Time.tm_min, &Time.tm_sec) != 6)
        return 0;
    Time.tm_year -= 1900;
    Time.tm_mon--;
    return mktime(&Time);
}
