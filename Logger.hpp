#pragma once

#include <iostream>
#include <string>
#include <mutex>

class LOGGER
{
private:
    std::string LogFilename;
    FILE *LogFile = NULL;
    std::mutex OutputMutex;
    void Output(std::string Type, std::string Style, std::string Data);

    friend class API_PROCEED;

public:
    LOGGER();
    ~LOGGER();

    void SetLogFilename(std::string LogFilename);

    void Debug(std::string Data);
    void Info(std::string Data);
    void Warning(std::string Data);
    void Error(std::string Data);
    void Fetal(std::string Data);
};

extern LOGGER Logger;
