#include "Users.hpp"
#include "Utilities.hpp"
#include "Privilege.hpp"

RESULT USERS::CreateUser(std::string Username, std::string Password, std::string Email, std::string Nickname, int Privilege)
{
    RETURN_IF_FAILED(DATABASE::INSERT("Users")
                         .Insert("Username", Username)
                         .Insert("Password", Password)
                         .Insert("Email", Email)
                         .Insert("Nickname", Nickname)
                         .Insert("Privilege", Privilege)
                         .Execute());
    CREATE_RESULT(true, "Add user succeed");
}
RESULT USERS::CheckUsernameAvailable(std::string Username)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Users")
                         .Select("UID")
                         .Where("Username", Username)
                         .Execute(
                             [](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(true, "Username available");
                                 CREATE_RESULT(false, "Username already exist");
                             }));
    CREATE_RESULT(true, "Username available");
}
RESULT USERS::CheckEmailAvailable(std::string Email)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Users")
                         .Select("UID")
                         .Where("Email", Email)
                         .Execute(
                             [](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(true, "Email available");
                                 CREATE_RESULT(false, "Email already exist");
                             }));
    CREATE_RESULT(true, "Email available");
}
RESULT USERS::CheckPasswordCorrect(std::string Username, std::string Password, int &UID)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Users")
                         .Select("UID")
                         .Where("Username", Username)
                         .Where("Password", Password)
                         .Execute(
                             [&UID](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "Username or password incorrect");
                                 UID = atoi(Data[0]["UID"].c_str());
                                 CREATE_RESULT(true, "Login succeed");
                             }));
    CREATE_RESULT(true, "Login succeed")
}
RESULT USERS::IsAdmin(int UID, bool &Result)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Users")
                         .Select("Privilege")
                         .Where("UID", UID)
                         .Execute(
                             [&Result](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "No such user");
                                 Result = (atoi(Data[0]["Privilege"].c_str()) == PRIVILEGE_LEVEL::ADMIN);
                                 CREATE_RESULT(true, "Success");
                             }));
    CREATE_RESULT(true, "Success");
}
