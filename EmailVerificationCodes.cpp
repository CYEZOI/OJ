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

#include "EmailVerificationCodes.hpp"
#include "Database.hpp"
#include "Utilities.hpp"

void EMAIL_VERIFICATION_CODES::CreateEmailVerificationCode(std::string EmailAddress, std::string &VerificationCode)
{
    DATABASE::SELECT("EmailVerificationCodes")
        .Select("CreateTime")
        .Where("EmailAddress", EmailAddress)
        .Execute(
            [EmailAddress](auto Data)
            {
                if (Data.size() != 0)
                {
                    if (UTILITIES::StringToTime(Data[0]["CreateTime"]) + 60 > time(NULL))
                        throw EXCEPTION("Email verification code already exists");
                    DATABASE::DELETE("EmailVerificationCodes")
                        .Where("EmailAddress", EmailAddress)
                        .Execute();
                }
                            });
    VerificationCode = "";
    const std::string VerificationCodeCharList = "0123456789";
    for (int i = 0; i < 6; i++)
        VerificationCode.push_back(VerificationCodeCharList[rand() % VerificationCodeCharList.size()]);
    DATABASE::INSERT("EmailVerificationCodes")
        .Insert("EmailAddress", EmailAddress)
        .Insert("VerificationCode", VerificationCode)
        .Execute();
    }
void EMAIL_VERIFICATION_CODES::CheckEmailVerificationCode(std::string EmailAddress, std::string VerificationCode)
{
    DATABASE::SELECT("EmailVerificationCodes")
        .Select("CreateTime")
        .Where("EmailAddress", EmailAddress)
        .Where("VerificationCode", VerificationCode)
        .Execute(
            [EmailAddress, VerificationCode](auto Data)
            {
                if (Data.size() != 1)
                    throw EXCEPTION("Verification code invalid");
                if (UTILITIES::StringToTime(Data[0]["CreateTime"]) + 600 < time(NULL))
                {
                    DeleteEmailVerificationCode(EmailAddress);
                    throw EXCEPTION("Verification code expired");
                }
                            });
    }
void EMAIL_VERIFICATION_CODES::DeleteEmailVerificationCode(std::string EmailAddress)
{
    DATABASE::DELETE("EmailVerificationCodes")
        .Where("EmailAddress", EmailAddress)
        .Execute();
    }
