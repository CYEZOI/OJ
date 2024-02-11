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

#pragma once

#include <iostream>
#include <string>
#include <mutex>

class LOGGER
{
private:
    std::string LogFilename;
    FILE *LogFile = NULL;
    std::mutex OutputMutex;
    void Output(std::string Type, std::string Style, std::string Data);

    friend class API_PROCEED;

public:
    LOGGER();
    ~LOGGER();

    void SetLogFilename(std::string LogFilename);

    void Debug(std::string Data);
    void Info(std::string Data);
    void Warning(std::string Data);
    void Error(std::string Data);
    void Fetal(std::string Data);
};

extern LOGGER Logger;
