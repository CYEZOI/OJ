#ifndef REGEXES_HPP
#define REGEXES_HPP

#include <regex>
#include "Result.hpp"

class REGEXES
{
public:
    static RESULT CheckUsername(std::string Username);
    static RESULT CheckPassword(std::string Password);
    static RESULT CheckNickname(std::string Nickname);
    static RESULT CheckEmailAddress(std::string EmailAddress);
    static RESULT CheckVerifyCode(std::string VerifyCode);
};

#endif
