#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>
#include <vector>
#include "Result.hpp"

class DATABASE
{
public:
    class SELECT
    {
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
        SELECT &Order(std::string Column, bool Descending);
        SELECT &Limit(int Limits);
        SELECT &Offset(int Offsets);
        RESULT Execute(std::function<RESULT(std::vector<std::map<std::string, std::string>>)> Callback = nullptr);
    };
    class INSERT
    {
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
        RESULT Execute(std::function<void(int)> Callback = nullptr);
    };
    class DELETE
    {
    private:
        sql::Connection *Connection;
        std::string TableName;
        std::vector<std::pair<std::string, std::string>> Conditions;

    public:
        DELETE(std::string TableName);
        ~DELETE();
        DELETE &Where(std::string Column, std::string Value);
        RESULT Execute();
    };
    class UPDATE
    {
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
        RESULT Execute();
    };

    static sql::Connection *CreateConnection();
    static void CloseConnection(sql::Connection *Connection);
};

#endif
