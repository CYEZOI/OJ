#ifndef EMAIL_VERIFY_CODES_HPP
#define EMAIL_VERIFY_CODES_HPP

#include "Result.hpp"

class EMAIL_VERIFY_CODES
{
public:
    static RESULT CreateEmailVerifyCode(std::string EmailAddress, std::string &VerifyCode);
    static RESULT CheckEmailVerifyCode(std::string EmailAddress, std::string VerifyCode);
    static RESULT DeleteEmailVerifyCode(std::string EmailAddress);
};

#endif
