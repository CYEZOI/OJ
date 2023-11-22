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

#pragma once

#include "Exception.hpp"

/*
CREATE TABLE `Passkeys` (
    `UID` INT UNSIGNED NOT NULL,
    `Credential` VARCHAR(64) NOT NULL,
    `PublicKey` TEXT NOT NULL
);

CREATE TABLE `PasskeyChallenges` (
    `UID` INT UNSIGNED NOT NULL,
    `Challenge` VARCHAR(64) NOT NULL,
    `CreateTime` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
);
*/

class PASSKEY
{
public:
    static std::string CreateChallenge();
    static void DeleteChallenge(std::string ChallengeID);
    static void AddPasskey(std::string Challenge, int UID, std::string Credential, std::string PublicKey);
};
