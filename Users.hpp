#ifndef USERS_HPP
#define USERS_HPP

#include "Result.hpp"
#include "Regexes.hpp"
#include "Database.hpp"

class USERS
{
private:
public:
    static RESULT CreateUser(std::string Username, std::string Password, std::string Email, std::string Nickname = "", int Privilege = 0);
    static RESULT CheckUsernameAvailable(std::string Username);
    static RESULT CheckPasswordCorrect(std::string Username, std::string Password, int &UID);
    static RESULT IsAdmin(std::string Username);
};

#endif
