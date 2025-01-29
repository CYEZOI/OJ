/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2025  langningchen

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

#pragma once

#include "Database.hpp"
#include "Exception.hpp"
#include "Regexes.hpp"
#include "Role.hpp"
#include "User.hpp"

class USERS {
  public:
    static std::string HashPassword(std::string Password);
    static void AddUser(std::string Username, std::string Nickname, std::string HashedPassword, std::string EmailAddress, int Role);
    static void CheckUsernameAvailable(std::string Username);
    static void CheckEmailAvailable(std::string EmailAddress);
    static void CheckPasswordCorrect(std::string Username, std::string HashedPassword, int &UID);
    static bool IsAdmin(int UID);
    static void UpdateUser(int UID, std::string Username, std::string Nickname, std::string HashedPassword, std::string EmailAddress, USER_ROLE Role);
    static void UpdateUserPassword(int UID, std::string HashedPassword);
    static void DeleteUser(int UID);
    static USER GetUser(int UID);
    static int GetUIDByEmailAddress(std::string EmailAddress);
};
