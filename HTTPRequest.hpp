#pragma once

#include "Logger.hpp"
#include <string>
#include <map>

class WEB_DATA_PROCEED;

class HTTP_REQUEST
{
private:
    std::string Version;
    std::string Verb;
    std::string Path;
    std::map<std::string, std::string> Headers;
    std::string Body;

    friend class WEB_DATA_PROCEED;

public:
    HTTP_REQUEST(std::string Data);
};
