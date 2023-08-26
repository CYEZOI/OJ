#pragma once

#include "Logger.hpp"
#include "Result.hpp"
#include "configor/json.hpp"

class SETTINGS
{
private:
    std::string DatabaseHost;
    int DatabasePort;
    std::string DatabaseUsername;
    std::string DatabasePassword;
    std::string DatabaseName;

    friend class API_PROCEED;
    friend class DATABASE;

public:
    RESULT Init();
    static RESULT GetSettings(std::string Key, std::string &Value);
    static RESULT GetSettings(std::string Key, int &Value);
    static RESULT GetSettings(configor::json &Value);
    static RESULT SetSettings(std::string Key, std::string Value);
    static RESULT SetSettings(std::string Key, int Value);
    static RESULT SetSettings(configor::json Value);
};

extern SETTINGS Settings;
