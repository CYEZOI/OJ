#ifndef USERS_HPP
#define USERS_HPP

#include "User.hpp"
#include "Result.hpp"
#include "Regexes.hpp"
#include "Database.hpp"

class USERS
{
private:
public:
    static RESULT AddUser(std::string Username, std::string Password, std::string Email, std::string Nickname = "", int Privilege = 0);
    static RESULT CheckUsernameAvailable(std::string Username);
    static RESULT CheckEmailAvailable(std::string Email);
    static RESULT CheckPasswordCorrect(std::string Username, std::string Password, int &UID);
    static RESULT IsAdmin(int UID, bool &Result);
    static RESULT GetUser(int UID, USER &User);
};

#endif
