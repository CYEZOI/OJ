#ifndef WEB_DATA_PROCEED_HPP
#define WEB_DATA_PROCEED_HPP

#include <string>
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "configor/json.hpp"

class WEB_DATA_PROCEED
{
private:
    const configor::json BaseJSON = configor::json::parse("{\"Success\": false, \"Message\": \"\", \"Data\": {}}");
    bool CheckTypes(configor::json JSON, std::vector<std::pair<std::string, configor::config_value_type>> Types);
    configor::json Login(std::string Username, std::string Password);
    configor::json CheckUsernameAvailable(std::string Username);
    configor::json SendVerifyCode(std::string EmailAddress);
    configor::json Register(std::string Username, std::string Password, std::string EmailAddress, std::string VerifyCode);
    configor::json CheckTokenAvailable(std::string Token);
    configor::json GetSubmission(int SID);
    configor::json Submit(std::string PID, bool EnableO2, std::string Code);
    configor::json GetProblem(std::string PID);

    void TestAddProblem();
    void TestSubmit();

public:
    HTTP_RESPONSE Proceed(HTTP_REQUEST HTTPRequest);
};

#endif
