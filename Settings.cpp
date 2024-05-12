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

#include "Settings.hpp"
#include "Utilities.hpp"
#include "Database.hpp"
#include <unistd.h>
#include <fstream>
#include <vector>
#include <sys/stat.h>

void SETTINGS::Init()
{
    UTILITIES::LoadFile("/etc/OJ/DatabaseHost", DatabaseHost);
    UTILITIES::LoadFile("/etc/OJ/DatabasePort", DatabasePort);
    UTILITIES::LoadFile("/etc/OJ/DatabaseUsername", DatabaseUsername);
    UTILITIES::LoadFile("/etc/OJ/DatabasePassword", DatabasePassword);
    UTILITIES::LoadFile("/etc/OJ/DatabaseName", DatabaseName);
    DatabaseHost = UTILITIES::RemoveSpaces(DatabaseHost);
    DatabaseUsername = UTILITIES::RemoveSpaces(DatabaseUsername);
    DatabasePassword = UTILITIES::RemoveSpaces(DatabasePassword);
    DatabaseName = UTILITIES::RemoveSpaces(DatabaseName);
}

void SETTINGS::GetSettings(std::string Key, std::string &Value)
{
    DATABASE::SELECT("Settings")
        .Where("Key", Key)
        .Execute(
            [Key, &Value](auto Data)
            {
                if (Data.size() == 0)
                {
                    throw EXCEPTION("Settings not found");
                }
                Value = Data[0]["Value"];
            });
}
void SETTINGS::GetSettings(std::string Key, int &Value)
{
    std::string ValueString;
    GetSettings(Key, ValueString);
    Value = std::stoi(ValueString);
}
void SETTINGS::GetSettings(configor::json &Value)
{
    DATABASE::SELECT("Settings")
        .Execute(
            [&Value](auto Data)
            {
                for (auto &Row : Data)
                    Value[Row["Key"]] = Row["Value"];
            });
}
void SETTINGS::SetSettings(std::string Key, std::string Value)
{
    DATABASE::SELECT("Settings")
        .Where("Key", Key)
        .Execute(
            [Key, &Value](auto Data)
            {
                if (Data.size() == 0)
                {
                    DATABASE::INSERT("Settings")
                        .Insert("Key", Key)
                        .Insert("Value", Value)
                        .Execute();
                }
                else
                {
                    DATABASE::UPDATE("Settings")
                        .Set("Value", Value)
                        .Where("Key", Key)
                        .Execute();
                }
            });
}
void SETTINGS::SetSettings(std::string Key, int Value)
{
    return SetSettings(Key, std::to_string(Value));
}
void SETTINGS::SetSettings(configor::json Value)
{
    DATABASE::TRUNCATE("Settings").Execute();
    for (auto i = Value.begin(); i != Value.end(); i++)
    {
        DATABASE::INSERT("Settings")
            .Insert("Key", i.key())
            .Insert("Value", i.value().as_string())
            .Execute();
    }
}

SETTINGS Settings;
