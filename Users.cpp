/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2024  langningchen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**********************************************************************/

#include "Users.hpp"
#include "Role.hpp"
#include "Settings.hpp"
#include "Utilities.hpp"
#include <openssl/sha.h>

std::string USERS::HashPassword(std::string Password) {
    std::string Salt1, Salt2;
    SETTINGS::GetSettings("PasswordSalt1", Salt1);
    SETTINGS::GetSettings("PasswordSalt2", Salt2);
    std::string SaltedPassword = Salt1 + Password + Salt2;
    unsigned char Hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)SaltedPassword.c_str(), SaltedPassword.length(), Hash);
    std::stringstream StringStream;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        StringStream << std::hex << (int)Hash[i];
    return StringStream.str();
}

void USERS::AddUser(std::string Username, std::string Nickname, std::string HashedPassword, std::string EmailAddress, int Role) {
    DATABASE::INSERT("Users")
        .Insert("Username", Username)
        .Insert("Password", HashedPassword)
        .Insert("Nickname", Nickname)
        .Insert("EmailAddress", EmailAddress)
        .Insert("Role", Role)
        .Execute();
}
void USERS::CheckUsernameAvailable(std::string Username) {
    DATABASE::SELECT("Users")
        .Select("UID")
        .Where("Username", Username)
        .Execute(
            [](auto Data) {
                if (Data.size() != 0)
                    throw EXCEPTION("Username already exist");
            });
}
void USERS::CheckEmailAvailable(std::string EmailAddress) {
    DATABASE::SELECT("Users")
        .Select("UID")
        .Where("EmailAddress", EmailAddress)
        .Execute(
            [](auto Data) {
                if (Data.size() != 0)
                    throw EXCEPTION("Email already exist");
            });
}
void USERS::CheckPasswordCorrect(std::string Username, std::string HashedPassword, int &UID) {
    DATABASE::SELECT("Users")
        .Select("UID")
        .Where("Username", Username)
        .Where("Password", HashedPassword)
        .Execute(
            [&UID](auto Data) {
                if (Data.size() == 0)
                    throw EXCEPTION("Username or password incorrect");
                UID = std::stoi(Data[0]["UID"]);
            });
}
bool USERS::IsAdmin(int UID) {
    bool IsAdmin;
    DATABASE::SELECT("Users")
        .Select("Role")
        .Where("UID", UID)
        .Execute(
            [&IsAdmin](auto Data) {
                if (Data.size() == 0)
                    throw EXCEPTION("No such user");
                IsAdmin = (std::stoi(Data[0]["Role"]) == USER_ROLE::USER_ROLE_ADMIN);
            });
    return IsAdmin;
}
void USERS::UpdateUser(int UID, std::string Username, std::string Nickname, std::string HashedPassword, std::string EmailAddress, USER_ROLE Role) {
    DATABASE::UPDATE("Users")
        .Set("Username", Username)
        .Set("Nickname", Nickname)
        .Set("Password", HashedPassword)
        .Set("EmailAddress", EmailAddress)
        .Set("Role", Role)
        .Where("UID", UID)
        .Execute();
}
void USERS::UpdateUserPassword(int UID, std::string HashedPassword) {
    DATABASE::UPDATE("Users")
        .Set("Password", HashedPassword)
        .Where("UID", UID)
        .Execute();
}
void USERS::DeleteUser(int UID) {
    DATABASE::DELETE("Users")
        .Where("UID", UID)
        .Execute();
}
USER USERS::GetUser(int UID) {
    USER User;
    DATABASE::SELECT("Users")
        .Select("UID")
        .Select("Username")
        .Select("EmailAddress")
        .Select("Nickname")
        .Select("Role")
        .Where("UID", UID)
        .Execute(
            [&User](auto Data) {
                if (Data.size() == 0)
                    throw EXCEPTION("No such user");
                User.UID = std::stoi(Data[0]["UID"]);
                User.Username = Data[0]["Username"];
                User.EmailAddress = Data[0]["EmailAddress"];
                User.Nickname = Data[0]["Nickname"];
                User.Role = std::stoi(Data[0]["Role"]);
            });
    return User;
}
int USERS::GetUIDByEmailAddress(std::string EmailAddress) {
    int UID = 0;
    DATABASE::SELECT("Users")
        .Select("UID")
        .Where("EmailAddress", EmailAddress)
        .Execute(
            [&UID](auto Data) {
                if (Data.size() == 0)
                    throw EXCEPTION("No such user");
                UID = std::stoi(Data[0]["UID"]);
            });
    return UID;
}
