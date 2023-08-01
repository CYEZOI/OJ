#pragma once

#include "Result.hpp"

class EMAIL_Verification_CODES
{
public:
    static RESULT CreateEmailVerificationCode(std::string EmailAddress, std::string &VerificationCode);
    static RESULT CheckEmailVerificationCode(std::string EmailAddress, std::string VerificationCode);
    static RESULT DeleteEmailVerificationCode(std::string EmailAddress);
};
