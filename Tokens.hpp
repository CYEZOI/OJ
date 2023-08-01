#pragma once

#include "Result.hpp"

class TOKENS
{
public:
    static RESULT CreateToken(int UID, std::string &TokenValue);
    static RESULT CheckToken(std::string TokenValue);
    static RESULT DeleteToken(std::string TokenValue);
    static RESULT GetUID(std::string TokenValue, int &UID);
};
