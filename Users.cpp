#include "Users.hpp"
#include "Utilities.hpp"

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
RESULT IsAdmin(std::string Username)
{
    RETURN_IF_FAILED(DATABASE::SELECT("Users")
                         .Select("Privilege")
                         .Where("Username", Username)
                         .Execute(
                             [](auto Data)
                             {
                                 if (Data.size() == 0)
                                     CREATE_RESULT(false, "No such user");
                                 if (atoi(Data[0]["Privilege"].c_str()) == 1)
                                     CREATE_RESULT(true, "Admin");
                                 CREATE_RESULT(false, "Not admin");
                             }));
    CREATE_RESULT(false, "Not admin");
}
