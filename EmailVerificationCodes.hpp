#ifndef EMAIL_Verification_CODES_HPP
#define EMAIL_Verification_CODES_HPP

#include "Result.hpp"

class EMAIL_Verification_CODES
{
public:
    static RESULT CreateEmailVerificationCode(std::string EmailAddress, std::string &VerificationCode);
    static RESULT CheckEmailVerificationCode(std::string EmailAddress, std::string VerificationCode);
    static RESULT DeleteEmailVerificationCode(std::string EmailAddress);
};

#endif
