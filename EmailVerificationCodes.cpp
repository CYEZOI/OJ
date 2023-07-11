#include "EmailVerificationCodes.hpp"
#include "Database.hpp"
#include "Utilities.hpp"

RESULT EMAIL_Verification_CODES::CreateEmailVerificationCode(std::string EmailAddress, std::string &VerificationCode)
{
    RETURN_IF_FAILED(DATABASE::SELECT("EmailVerificationCodes")
                         .Select("CreateTime")
                         .Where("EmailAddress", EmailAddress)
                         .Execute(
                             [EmailAddress](auto Data)
                             {
                                 if (Data.size() != 0)
                                 {
                                     RETURN_IF_FALSE(UTILITIES::StringToTime(Data[0]["CreateTime"]) + 60 <= time(NULL), "Email Verification code already exists")
                                     RETURN_IF_FAILED(DATABASE::DELETE("EmailVerificationCodes")
                                                          .Where("EmailAddress", EmailAddress)
                                                          .Execute())
                                 }
                                 CREATE_RESULT(true, "No email Verification code");
                             }))
    VerificationCode = "";
    const std::string VerificationCodeCharList = "0123456789";
    for (int i = 0; i < 6; i++)
        VerificationCode.push_back(VerificationCodeCharList[rand() % VerificationCodeCharList.size()]);
    RETURN_IF_FAILED(DATABASE::INSERT("EmailVerificationCodes")
                         .Insert("EmailAddress", EmailAddress)
                         .Insert("VerificationCode", VerificationCode)
                         .Execute())
    CREATE_RESULT(true, "Create email Verification code succeed");
}
RESULT EMAIL_Verification_CODES::CheckEmailVerificationCode(std::string EmailAddress, std::string VerificationCode)
{
    RETURN_IF_FAILED(DATABASE::SELECT("EmailVerificationCodes")
                         .Select("CreateTime")
                         .Where("EmailAddress", EmailAddress)
                         .Where("VerificationCode", VerificationCode)
                         .Execute(
                             [EmailAddress, VerificationCode](auto Data)
                             {
                                 CREATE_RESULT_IF_FALSE(Data.size() == 1, "Verification code invalid");
                                 CREATE_RESULT_IF_FALSE_WITH_OPERATION(UTILITIES::StringToTime(Data[0]["CreateTime"]) + 600 >= time(NULL), "Verification code expired", DeleteEmailVerificationCode(EmailAddress))
                                 CREATE_RESULT(true, "Verification code valid");
                             }))
    CREATE_RESULT(true, "Verification code valid");
}
RESULT EMAIL_Verification_CODES::DeleteEmailVerificationCode(std::string EmailAddress)
{
    RETURN_IF_FAILED(DATABASE::DELETE("EmailVerificationCodes")
                         .Where("EmailAddress", EmailAddress)
                         .Execute())
    CREATE_RESULT(true, "Delete email Verification code succeed");
}
