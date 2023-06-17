#include "Regexes.hpp"

RESULT REGEXES::CheckUsername(std::string Username)
{
    if (std::regex_match(Username, std::regex("^[0-9a-zA-Z]{4,16}$")))
        CREATE_RESULT(true, "Username valid")
    CREATE_RESULT(false, "Username invalid")
}
RESULT REGEXES::CheckPassword(std::string Password)
{
    if (!std::regex_match(Password, std::regex("^([^a-z]+|[^A-Z]+|[^0-9]+|[a-zA-Z0-9]+)$")))
        CREATE_RESULT(true, "Password valid")
    CREATE_RESULT(false, "Password invalid")
}
RESULT REGEXES::CheckNickname(std::string Nickname)
{
    if (std::regex_match(Nickname, std::regex("^.{4,16}$")))
        CREATE_RESULT(true, "Nickname valid")
    CREATE_RESULT(false, "Nickname invalid")
}
RESULT REGEXES::CheckEmailAddress(std::string EmailAddress)
{
    if (std::regex_match(EmailAddress, std::regex("^([a-zA-Z0-9_-])+@([a-zA-Z0-9_-])+(.[a-zA-Z0-9_-])+$")))
        CREATE_RESULT(true, "Email address valid")
    CREATE_RESULT(false, "Email address invalid")
}
RESULT REGEXES::CheckVerifyCode(std::string VerifyCode)
{
    if (std::regex_match(VerifyCode, std::regex("^[0-9]{6}$")))
        CREATE_RESULT(true, "Verify code valid")
    CREATE_RESULT(false, "Verify code invalid")
}
