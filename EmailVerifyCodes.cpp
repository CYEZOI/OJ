#include "EmailVerifyCodes.hpp"
#include "Database.hpp"
#include "Utilities.hpp"

RESULT EMAIL_VERIFY_CODES::CreateEmailVerifyCode(std::string EmailAddress, std::string &VerifyCode)
{
    RETURN_IF_FAILED(DATABASE::SELECT("EmailVerifyCodes")
                         .Select("CreateTime")
                         .Where("EmailAddress", EmailAddress)
                         .Execute(
                             [EmailAddress](auto Data)
                             {
                                 if (Data.size() != 0)
                                 {
                                     RETURN_IF_FALSE(UTILITIES::StringToTime(Data[0]["CreateTime"]) + 60 <= time(NULL), "Email verify code already exists")
                                     RETURN_IF_FAILED(DATABASE::DELETE("EmailVerifyCodes")
                                                          .Where("EmailAddress", EmailAddress)
                                                          .Execute())
                                 }
                                 CREATE_RESULT(true, "No email verify code");
                             }))
    VerifyCode = "";
    const std::string VerifyCodeCharList = "0123456789";
    for (int i = 0; i < 6; i++)
        VerifyCode.push_back(VerifyCodeCharList[rand() % VerifyCodeCharList.size()]);
    RETURN_IF_FAILED(DATABASE::INSERT("EmailVerifyCodes")
                         .Insert("EmailAddress", EmailAddress)
                         .Insert("VerifyCode", VerifyCode)
                         .Execute())
    CREATE_RESULT(true, "Create email verify code succeed");
}
RESULT EMAIL_VERIFY_CODES::CheckEmailVerifyCode(std::string EmailAddress, std::string VerifyCode)
{
    RETURN_IF_FAILED(DATABASE::SELECT("EmailVerifyCodes")
                         .Select("CreateTime")
                         .Where("EmailAddress", EmailAddress)
                         .Where("VerifyCode", VerifyCode)
                         .Execute(
                             [EmailAddress, VerifyCode](auto Data)
                             {
                                 CREATE_RESULT_IF_FALSE(Data.size() == 1, "Verify code invalid");
                                 CREATE_RESULT_IF_FALSE_WITH_OPERATION(UTILITIES::StringToTime(Data[0]["CreateTime"]) + 600 >= time(NULL), "Verify code expired", DeleteEmailVerifyCode(EmailAddress))
                                 CREATE_RESULT(true, "Verify code valid");
                             }))
    CREATE_RESULT(true, "Verify code valid");
}
RESULT EMAIL_VERIFY_CODES::DeleteEmailVerifyCode(std::string EmailAddress)
{
    RETURN_IF_FAILED(DATABASE::DELETE("EmailVerifyCodes")
                         .Where("EmailAddress", EmailAddress)
                         .Execute())
    CREATE_RESULT(true, "Delete email verify code succeed");
}
