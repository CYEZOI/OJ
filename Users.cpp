/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2023  langningchen

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
#include "Utilities.hpp"
#include "Role.hpp"
#include "Settings.hpp"
#include <openssl/sha.h>

RESULT USERS::HashPassword(std::string Password, std::string &HashedPassword)
{
    std::string Salt1, Salt2;
    RETURN_IF_FAILED(SETTINGS::GetSettings("PasswordSalt1", Salt1));
    RETURN_IF_FAILED(SETTINGS::GetSettings("PasswordSalt2", Salt2));
    std::string SaltedPassword = Salt1 + Password + Salt2;
    unsigned char Hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)SaltedPassword.c_str(), SaltedPassword.length(), Hash);
    std::stringstream StringStream;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        StringStream << std::hex << (int)Hash[i];
    HashedPassword = StringStream.str();
    CREATE_RESULT(true, "Hash password succeeds");
}

RESULT USERS::AddUser(std::string Username, std::string Nickname, std::string HashedPassword, std::string EmailAddress, int Role)
{
    RETURN_IF_FAILED(DATABASE::INSERT("Users")
                         .Insert("Username", Username)
                         .Insert("Password", HashedPassword)
                         .Insert("Nickname", Nickname)
                         .Insert("EmailAddress", EmailAddress)
                         .Insert("Role", Role)
                         .Execute());
    CREATE_RESULT(true, "Add user succeeds");
}
RESULT USERS::CheckUsernameAvailable(std::string Username)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Users")
                         .Select("UID")
                         .Where("Username", Username)
                         .Execute(
                             [](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(true, "Username available");
                                 CREATE_RESULT(false, "Username already exist");
                             }));
    CREATE_RESULT(true, "Username available");
}
RESULT USERS::CheckEmailAvailable(std::string EmailAddress)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Users")
                         .Select("UID")
                         .Where("EmailAddress", EmailAddress)
                         .Execute(
                             [](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(true, "Email available");
                                 CREATE_RESULT(false, "Email already exist");
                             }));
    CREATE_RESULT(true, "Email available");
}
RESULT USERS::CheckPasswordCorrect(std::string Username, std::string HashedPassword, int &UID)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Users")
                         .Select("UID")
                         .Where("Username", Username)
                         .Where("Password", HashedPassword)
                         .Execute(
                             [&UID](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "Username or password incorrect");
                                 UID = atoi(Data[0]["UID"].c_str());
                                 CREATE_RESULT(true, "Login succeeds");
                             }));
    CREATE_RESULT(true, "Login succeeds")
}
RESULT USERS::IsAdmin(int UID, bool &Result)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Users")
                         .Select("Role")
                         .Where("UID", UID)
                         .Execute(
                             [&Result](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "No such user");
                                 Result = (atoi(Data[0]["Role"].c_str()) == USER_ROLE::USER_ROLE_ADMIN);
                                 CREATE_RESULT(true, "Success");
                             }));
    CREATE_RESULT(true, "Success");
}
RESULT USERS::UpdateUser(int UID, std::string Username, std::string Nickname, std::string HashedPassword, std::string EmailAddress, USER_ROLE Role)
{
    RETURN_IF_FAILED(DATABASE::UPDATE("Users")
                         .Set("Username", Username)
                         .Set("Nickname", Nickname)
                         .Set("Password", HashedPassword)
                         .Set("EmailAddress", EmailAddress)
                         .Set("Role", Role)
                         .Where("UID", UID)
                         .Execute());
    CREATE_RESULT(true, "Update user succeeds");
}
RESULT USERS::DeleteUser(int UID)
{
    RETURN_IF_FAILED(DATABASE::DELETE("Users")
                         .Where("UID", UID)
                         .Execute());
    CREATE_RESULT(true, "Delete user succeeds");
}
RESULT USERS::GetUser(int UID, USER &User)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Users")
                         .Select("UID")
                         .Select("Username")
                         .Select("EmailAddress")
                         .Select("Nickname")
                         .Select("Role")
                         .Where("UID", UID)
                         .Execute(
                             [&User](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "No such user");
                                 User.UID = atoi(Data[0]["UID"].c_str());
                                 User.Username = Data[0]["Username"];
                                 User.EmailAddress = Data[0]["EmailAddress"];
                                 User.Nickname = Data[0]["Nickname"];
                                 User.Role = atoi(Data[0]["Role"].c_str());
                                 CREATE_RESULT(true, "Success");
                             }));
    CREATE_RESULT(true, "Success");
}
