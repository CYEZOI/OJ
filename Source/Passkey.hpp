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

#include "Exception.hpp"
#include <configor/json.hpp>
#include <vector>

class PASSKEY {
  private: // 定义内部数据结构
    class AuthenticatorFlags {
      private:
        bool up;
        bool uv;
        bool be;
        bool bs;
        bool at;
        bool ed;
        uint8_t flagsInt;

        friend class PASSKEY;

      public:
        AuthenticatorFlags() = default;
        AuthenticatorFlags(uint8_t flagsInt);
        std::string toString() const;
    };

    class ParsedData {
      private:
        std::string rpIdHash;
        std::vector<uint8_t> flagsBuf;
        AuthenticatorFlags flags;
        uint32_t counter;
        std::vector<uint8_t> counterBuf;
        std::vector<uint8_t> aaguid;
        std::vector<uint8_t> credentialID;
        std::vector<uint8_t> credentialPublicKey;
        std::vector<uint8_t> extensionsDataBuffer;

        friend class PASSKEY;

      public:
        ParsedData(std::vector<uint8_t> AuthData);
        void logDebugInfo() const;
    };

  public:
    static std::string CreateChallenge();
    static void DeleteChallenge(std::string Challenge);
    static void CreatePasskey(int UID, configor::json Credential);
    static std::string GetPasskey(int UID, std::string Credential);
};
