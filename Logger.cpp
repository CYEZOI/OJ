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

#include "Logger.hpp"
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

LOGGER::LOGGER() { SetLogFilename("Log.log"); }
LOGGER::~LOGGER() { fclose(LogFile); }

void LOGGER::SetLogFilename(std::string LogFilename)
{
    LogFile = fopen(LogFilename.c_str(), "a+");
    if (LogFile == nullptr)
    {
        LogFile = stdout;
        this->LogFilename = "";
        Error("Failed to open log file: " + LogFilename);
    }
    else
        this->LogFilename = LogFilename;
}

void LOGGER::Output(std::string Type, std::string Style, std::string Data)
{
    if (LogFile == nullptr)
        LogFile = stdout;

    if (Data.length() > 65535)
        Data = Data.substr(0, 65535) + " ... (truncated)";

    struct timeval CurrentSecond;
    gettimeofday(&CurrentSecond, nullptr);
    int MilliSecond = CurrentSecond.tv_usec / 1000;

    char CurrentTime[80] = {0};
    struct tm TempTime;
    localtime_r(&CurrentSecond.tv_sec, &TempTime);
    strftime(CurrentTime, sizeof(CurrentTime), "%Y-%m-%d %H:%M:%S", &TempTime);

    char* Buffer = new char[Data.length() + 100];
    if (Buffer == nullptr)
    {
        std::cout << "Failed to allocate memory for log buffer" << std::endl;
        return;
    }
    sprintf(Buffer, "\033[%sm%s[%s.%03d][%d-%d] %s\033[0m\n",
            Style.c_str(), Type.c_str(),
            CurrentTime, MilliSecond,
            getpid(), gettid(),
            Data.c_str());

    OutputMutex.lock();
    fprintf(LogFile, "%s", Buffer);
    fflush(LogFile);
    errno = 0;
    if (Type == "W" || Type == "E")
        printf("%s", Buffer);
    OutputMutex.unlock();
    delete Buffer;
}

void LOGGER::Debug(std::string Data)
{
    // Output("D", "36", Data);
}
void LOGGER::Info(std::string Data) { Output("I", "32", Data); }
void LOGGER::Warning(std::string Data) { Output("W", "33", Data); }
void LOGGER::Error(std::string Data) { Output("E", "31", Data + ", " + std::to_string(errno) + ": " + std::string(strerror(errno))); }
void LOGGER::Fetal(std::string Data)
{
    Output("F", "1;4;5;31", Data);
    exit(1);
}

LOGGER Logger;
