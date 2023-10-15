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

#define CREATE_RESULT(Success, Message)                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                  \
        return RESULT((Success), std::string(Message), std::string(Message) + "\n" + std::to_string(errno) + ": " + std::string(strerror(errno)) + "\nCall trace: \n" + __FILE__ + ":" + std::to_string(__LINE__) + " " + __PRETTY_FUNCTION__ + "\n"); \
    }

#define CREATE_RESULT_IF_FALSE(Bool, Message) \
    {                                         \
        if (!(Bool))                          \
            CREATE_RESULT(false, (Message))   \
    }

#define CREATE_RESULT_IF_FALSE_WITH_OPERATION(Bool, Message, AdditionalOperation) \
    {                                                                             \
        if (!(Bool))                                                              \
        {                                                                         \
            (AdditionalOperation);                                                \
            CREATE_RESULT(false, (Message))                                       \
        }                                                                         \
    }

#define OUTPUT_IF_FAILED(NewResult)                         \
    {                                                       \
        RESULT _Result = (NewResult);                       \
        if (!_Result.Success)                               \
            std::cout << _Result.DebugMessage << std::endl; \
    }

#define RETURN_IF_FAILED(NewResult)                                                                                                                               \
    {                                                                                                                                                             \
        RESULT _Result = (NewResult);                                                                                                                             \
        if (!_Result.Success)                                                                                                                                     \
            return RESULT(_Result.Success, _Result.Message, _Result.DebugMessage + __FILE__ + ":" + std::to_string(__LINE__) + " " + __PRETTY_FUNCTION__ + "\n"); \
    }

#define RETURN_IF_FALSE(Bool, Message)      \
    {                                       \
        if (!(Bool))                        \
            CREATE_RESULT(false, (Message)) \
    }

#define RETURN_IF_FAILED_WITH_OPERATION(NewResult, AdditionalOperation)                                                                                           \
    {                                                                                                                                                             \
        RESULT _Result = (NewResult);                                                                                                                             \
        if (!_Result.Success)                                                                                                                                     \
        {                                                                                                                                                         \
            (AdditionalOperation);                                                                                                                                \
            return RESULT(_Result.Success, _Result.Message, _Result.DebugMessage + __FILE__ + ":" + std::to_string(__LINE__) + " " + __PRETTY_FUNCTION__ + "\n"); \
        }                                                                                                                                                         \
    }

#define CREATE_JSON(Success, Message)             \
    {                                             \
        configor::json _ResponseJSON;             \
        _ResponseJSON["Success"] = (Success);     \
        _ResponseJSON["Message"] = (Message);     \
        _ResponseJSON["Data"] = configor::json(); \
        return _ResponseJSON;                     \
    }

#define RETURN_JSON_IF_FAILED(NewResult)        \
    {                                           \
        RESULT _Result = (NewResult);           \
        if (!_Result.Success)                   \
            CREATE_JSON(false, _Result.Message) \
    }

#define RETURN_JSON_IF_FALSE(Bool, Message) \
    {                                       \
        if (!(Bool))                        \
            CREATE_JSON(false, (Message))   \
    }

#define RETURN_JSON_IF_FALSE_WITH_OPERATION(Bool, Message, AdditionalOperation) \
    {                                                                           \
        if (!(Bool))                                                            \
        {                                                                       \
            (AdditionalOperation);                                              \
            CREATE_JSON(false, (Message))                                       \
        }                                                                       \
    }

#define THROW_STRING_ERROR_IF_FAILED(NewResult) \
    {                                           \
        RESULT _Result = (NewResult);           \
        if (!_Result.Success)                   \
            throw std::string(_Result.Message); \
    }

#include <string>
#include <string.h>
#include "Logger.hpp"

using namespace std::literals::string_literals;

class RESULT
{
public:
    bool Success = false;
    std::string Message;
    std::string DebugMessage;

    RESULT();
    RESULT(bool Success, std::string Message, std::string DebugMessage);
};
