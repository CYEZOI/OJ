#pragma once

#include "User.hpp"
#include "Result.hpp"
#include "Regexes.hpp"
#include "Database.hpp"
#include "Privilege.hpp"

class USERS
{
private:
public:
    static RESULT AddUser(std::string Username, std::string Password, std::string Email, std::string Nickname = "", int Privilege = PRIVILEGE_LEVEL::PRIVILEGE_LEVEL_USER);
    static RESULT CheckUsernameAvailable(std::string Username);
    static RESULT CheckEmailAvailable(std::string Email);
    static RESULT CheckPasswordCorrect(std::string Username, std::string Password, int &UID);
    static RESULT IsAdmin(int UID, bool &Result);
    static RESULT GetUser(int UID, USER &User);
};
