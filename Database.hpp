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
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <string>
#include <vector>

class DATABASE {
  public:
    class SELECT {
      private:
        bool UseOrder = false;
        sql::Connection *Connection;
        std::string TableName;
        std::vector<std::string> Columns;
        std::vector<std::pair<std::string, std::string>> Conditions;
        std::vector<std::pair<std::string, bool>> Orders;
        int Limits = 0;
        int Offsets = 0;

      public:
        SELECT(std::string TableName);
        ~SELECT();
        SELECT &Select(std::string Column);
        SELECT &Where(std::string Column, std::string Value);
        SELECT &Where(std::string Column, int Value);
        SELECT &Order(std::string Column, bool Ascending);
        SELECT &Limit(int Limits);
        SELECT &Offset(int Offsets);
        void Execute(std::function<void(std::vector<std::map<std::string, std::string>>)> Callback = nullptr);
    };
    class INSERT {
      private:
        sql::Connection *Connection;
        std::string TableName;
        std::vector<std::string> Columns;
        std::vector<std::string> Values;

      public:
        INSERT(std::string TableName);
        ~INSERT();
        INSERT &Insert(std::string Column, std::string Value);
        INSERT &Insert(std::string Column, int Value);
        void Execute(std::function<void(int)> Callback = nullptr);
    };
    class DELETE {
      private:
        sql::Connection *Connection;
        std::string TableName;
        std::vector<std::pair<std::string, std::string>> Conditions;

      public:
        DELETE(std::string TableName);
        ~DELETE();
        DELETE &Where(std::string Column, std::string Value);
        DELETE &Where(std::string Column, int Value);
        void Execute();
    };
    class UPDATE {
      private:
        sql::Connection *Connection;
        std::string TableName;
        std::vector<std::pair<std::string, std::string>> Columns;
        std::vector<std::pair<std::string, std::string>> Conditions;

      public:
        UPDATE(std::string TableName);
        ~UPDATE();
        UPDATE &Set(std::string Column, std::string Value);
        UPDATE &Set(std::string Column, int Value);
        UPDATE &Where(std::string Column, std::string Value);
        UPDATE &Where(std::string Column, int Value);
        void Execute();
    };
    class SIZE {
      private:
        sql::Connection *Connection;
        std::string TableName;

      public:
        SIZE(std::string TableName);
        void Execute(std::function<void(int)> Callback = nullptr);
    };
    class TRUNCATE {
      private:
        sql::Connection *Connection;
        std::string TableName;

      public:
        TRUNCATE(std::string TableName);
        void Execute();
    };

    static sql::Connection *CreateConnection();
    static void CloseConnection(sql::Connection *Connection);
};
