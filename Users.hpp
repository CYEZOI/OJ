#pragma once

#include "User.hpp"
#include "Result.hpp"
#include "Regexes.hpp"
#include "Database.hpp"
#include "Role.hpp"

class USERS
{
public:
    static RESULT HashPassword(std::string Password, std::string &HashedPassword);
    static RESULT AddUser(std::string Username, std::string Nickname, std::string HashedPassword, std::string EmailAddress, int Role);
    static RESULT CheckUsernameAvailable(std::string Username);
    static RESULT CheckEmailAvailable(std::string EmailAddress);
    static RESULT CheckPasswordCorrect(std::string Username, std::string HashedPassword, int &UID);
    static RESULT IsAdmin(int UID, bool &Result);
    static RESULT UpdateUser(int UID, std::string Username, std::string Nickname, std::string HashedPassword, std::string EmailAddress, USER_ROLE Role);
    static RESULT DeleteUser(int UID);
    static RESULT GetUser(int UID, USER &User);
};
