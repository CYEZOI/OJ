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

#include "HTTPRequest.hpp"
#include "Utilities.hpp"
#include "Settings.hpp"

RESULT HTTP_REQUEST::Parse(std::string Data)
{
    Data = UTILITIES::StringReplaceAll(Data, "\r", "");

    size_t VerbStartPosition = 0;
    size_t VerbEndPosition = Data.find(" ", VerbStartPosition);
    if (VerbEndPosition == std::string::npos)
        CREATE_RESULT(false, "Verb not found");
    Verb = Data.substr(VerbStartPosition, VerbEndPosition - VerbStartPosition);

    size_t PathStartPosition = VerbEndPosition + 1;
    size_t PathEndPosition = Data.find(" ", PathStartPosition);
    if (PathEndPosition == std::string::npos)
        CREATE_RESULT(false, "Path not found");
    Path = Data.substr(PathStartPosition, PathEndPosition - PathStartPosition);

    size_t VersionStartPosition = PathEndPosition + 1;
    size_t VersionEndPosition = Data.find("\n", VersionStartPosition);
    if (VersionEndPosition == std::string::npos)
        CREATE_RESULT(false, "Version not found");
    Version = Data.substr(VersionStartPosition, VersionEndPosition - VersionStartPosition);

    std::string Line;
    for (size_t i = VersionEndPosition + 1; i < Data.length(); i++)
    {
        if (Data[i] == '\n')
        {
            if (Line == "")
            {
                Body = Data.substr(i);
                break;
            }
            size_t NameStartPosition = 0;
            size_t NameEndPosition = Line.find(": ", NameStartPosition);
            if (NameEndPosition == std::string::npos)
                CREATE_RESULT(false, "Header data not found");
            std::string Name = Line.substr(NameStartPosition, NameEndPosition - NameStartPosition);

            size_t ValueStartPosition = NameEndPosition + 2;
            size_t ValueEndPosition = Line.size();
            std::string Value = Line.substr(ValueStartPosition, ValueEndPosition - ValueStartPosition);

            Headers[Name] = Value;

            Line = "";
        }
        else
            Line.push_back(Data[i]);
    }
    CREATE_RESULT(true, "HTTP parse succeeds");
}
