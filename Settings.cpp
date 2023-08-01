#include "Settings.hpp"
#include "Utilities.hpp"
#include "Database.hpp"
#include <unistd.h>
#include <fstream>
#include <vector>
#include <sys/stat.h>

RESULT SETTINGS::Init()
{
    RETURN_IF_FAILED(UTILITIES::LoadFile("/etc/OJ/DatabaseHost", DatabaseHost));
    RETURN_IF_FAILED(UTILITIES::LoadFile("/etc/OJ/DatabasePort", DatabasePort));
    RETURN_IF_FAILED(UTILITIES::LoadFile("/etc/OJ/DatabaseUsername", DatabaseUsername));
    RETURN_IF_FAILED(UTILITIES::LoadFile("/etc/OJ/DatabasePassword", DatabasePassword));
    RETURN_IF_FAILED(UTILITIES::LoadFile("/etc/OJ/DatabaseName", DatabaseName));
    DatabaseHost = UTILITIES::RemoveSpaces(DatabaseHost);
    DatabaseUsername = UTILITIES::RemoveSpaces(DatabaseUsername);
    DatabasePassword = UTILITIES::RemoveSpaces(DatabasePassword);
    DatabaseName = UTILITIES::RemoveSpaces(DatabaseName);
    CREATE_RESULT(true, "Settings init");
}

RESULT SETTINGS::GetSettings(std::string Key, std::string &Value)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Settings")
                         .Where("Key", Key)
                         .Execute(
                             [&Value](auto Data)
                             {
                                 if (Data.size() == 0)
                                 {
                                     CREATE_RESULT(false, "Settings not found");
                                 }
                                 Value = Data[0]["Value"];
                                 CREATE_RESULT(true, "Settings get");
                             }));
    CREATE_RESULT(true, "Settings get");
}
RESULT SETTINGS::GetSettings(std::string Key, int &Value)
{
    std::string ValueString;
    RETURN_IF_FAILED(GetSettings(Key, ValueString));
    Value = std::stoi(ValueString);
    CREATE_RESULT(true, "Settings get");
}
RESULT SETTINGS::GetSettings(configor::json &Value)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Settings")
                         .Execute(
                             [&Value](auto Data)
                             {
                                 for (auto &Row : Data)
                                     Value[Row["Key"]] = Row["Value"];
                                 CREATE_RESULT(true, "Settings get");
                             }));
    CREATE_RESULT(true, "Settings get");
}
RESULT SETTINGS::SetSettings(std::string Key, std::string Value)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Settings")
                         .Where("Key", Key)
                         .Execute(
                             [Key, &Value](auto Data)
                             {
                                 if (Data.size() == 0)
                                 {
                                     RETURN_IF_FAILED(DATABASE::INSERT("Settings")
                                                          .Insert("Key", Key)
                                                          .Insert("Value", Value)
                                                          .Execute());
                                 }
                                 else
                                 {
                                     RETURN_IF_FAILED(DATABASE::UPDATE("Settings")
                                                          .Set("Value", Value)
                                                          .Where("Key", Key)
                                                          .Execute());
                                 }
                                 CREATE_RESULT(true, "Settings set");
                             }));
    CREATE_RESULT(true, "Settings set");
}
RESULT SETTINGS::SetSettings(std::string Key, int Value)
{
    return SetSettings(Key, std::to_string(Value));
}
RESULT SETTINGS::SetSettings(configor::json Value)
{
    RETURN_IF_FAILED(DATABASE::TRUNCATE("Settings").Execute());
    for (auto i = Value.begin(); i != Value.end(); i++)
    {
        RETURN_IF_FAILED(DATABASE::INSERT("Settings")
                             .Insert("Key", i.key())
                             .Insert("Value", i.value().as_string())
                             .Execute());
    }
    CREATE_RESULT(true, "Settings set");
}

SETTINGS Settings;
