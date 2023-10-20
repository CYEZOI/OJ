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

#include "Logger.hpp"
#include "Exception.hpp"
#include <configor/json.hpp>

class SETTINGS
{
private:
    std::string DatabaseHost;
    int DatabasePort;
    std::string DatabaseUsername;
    std::string DatabasePassword;
    std::string DatabaseName;

    friend class API_PROCEED;
    friend class DATABASE;

public:
    void Init();
    static void GetSettings(std::string Key, std::string &Value);
    static void GetSettings(std::string Key, int &Value);
    static void GetSettings(configor::json &Value);
    static void SetSettings(std::string Key, std::string Value);
    static void SetSettings(std::string Key, int Value);
    static void SetSettings(configor::json Value);
};

extern SETTINGS Settings;
