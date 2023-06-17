#ifndef TOKENS_HPP
#define TOKENS_HPP

#include "Result.hpp"

class TOKENS
{
public:
    static RESULT CreateToken(int UID, std::string &TokenValue);
    static RESULT CheckToken(std::string TokenValue);
    static RESULT DeleteToken(std::string TokenValue);
};

#endif
