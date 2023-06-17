#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <mutex>

class LOGGER
{
private:
    std::string LogFileName;
    FILE *LogFile = NULL;
    std::mutex OutputMutex;
    void Output(std::string Type, std::string Style, std::string Data);

    friend class WEB_DATA_PROCEED;

public:
    LOGGER();
    ~LOGGER();

    void SetLogFileName(std::string LogFileName);

    void Debug(std::string Data);
    void Info(std::string Data);
    void Warning(std::string Data);
    void Error(std::string Data);
    void Fetal(std::string Data);
};

extern LOGGER Logger;

#endif
