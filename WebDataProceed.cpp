#include "WebDataProceed.hpp"
#include "Settings.hpp"
#include "TestCase.hpp"
#include "TestGroup.hpp"
#include "Regexes.hpp"
#include "Problems.hpp"
#include "Submission.hpp"
#include "JudgingList.hpp"
#include "Utilities.hpp"
#include "Users.hpp"
#include "Tokens.hpp"
#include "EmailVerifyCodes.hpp"
#include "Submissions.hpp"
#include <string>
#include <regex>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <list>
#include <algorithm>

bool WEB_DATA_PROCEED::CheckTypes(configor::json JSON, std::vector<std::pair<std::string, configor::config_value_type>> Types)
{
    for (auto i : Types)
        if (JSON["Data"][i.first].type() != i.second)
            return false;
    return true;
}
configor::json WEB_DATA_PROCEED::Login(std::string Username, std::string Password)
{
    RETURN_JSON_IF_FAILED(REGEXES::CheckUsername(Username))
    RETURN_JSON_IF_FAILED(REGEXES::CheckPassword(Password))
    int UID;
    RETURN_JSON_IF_FAILED(USERS::CheckPasswordCorrect(Username, Password, UID))
    std::string Token;
    RETURN_JSON_IF_FAILED(TOKENS::CreateToken(UID, Token))
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Login success";
    ResponseJSON["Data"]["Token"] = Token;
    return ResponseJSON;
}
configor::json WEB_DATA_PROCEED::CheckUsernameAvailable(std::string Username)
{
    RETURN_JSON_IF_FAILED(REGEXES::CheckUsername(Username))
    RETURN_JSON_IF_FAILED(USERS::CheckUsernameAvailable(Username))
    CREATE_JSON(true, "Username available");
}
configor::json WEB_DATA_PROCEED::SendVerifyCode(std::string EmailAddress)
{
    RETURN_JSON_IF_FAILED(REGEXES::CheckEmailAddress(EmailAddress))
    std::string VerifyCode;
    RETURN_JSON_IF_FAILED(EMAIL_VERIFY_CODES::CreateEmailVerifyCode(EmailAddress, VerifyCode))
    RETURN_JSON_IF_FAILED(UTILITIES::SendEmail(EmailAddress, "Verify Code", "Your verify code is " + VerifyCode))
    CREATE_JSON(true, "Send verify code success")
}
configor::json WEB_DATA_PROCEED::Register(std::string Username, std::string Password, std::string EmailAddress, std::string VerifyCode)
{
    RETURN_JSON_IF_FAILED(REGEXES::CheckUsername(Username))
    RETURN_JSON_IF_FAILED(REGEXES::CheckPassword(Password))
    RETURN_JSON_IF_FAILED(REGEXES::CheckEmailAddress(EmailAddress))
    RETURN_JSON_IF_FAILED(REGEXES::CheckVerifyCode(VerifyCode))
    RETURN_JSON_IF_FAILED(EMAIL_VERIFY_CODES::CheckEmailVerifyCode(EmailAddress, VerifyCode))
    RETURN_JSON_IF_FAILED(EMAIL_VERIFY_CODES::DeleteEmailVerifyCode(EmailAddress))
    RETURN_JSON_IF_FAILED(USERS::CheckUsernameAvailable(Username))
    RETURN_JSON_IF_FAILED(USERS::CreateUser(Username, Password, "", EmailAddress))
    CREATE_JSON(true, "Register success");
}
configor::json WEB_DATA_PROCEED::CheckTokenAvailable(std::string Token)
{
    RETURN_JSON_IF_FAILED(TOKENS::CheckToken(Token))
    CREATE_JSON(true, "Token available");
}
configor::json WEB_DATA_PROCEED::GetSubmission(int SID)
{
    SUBMISSION Submission;
    RETURN_JSON_IF_FAILED(SUBMISSIONS::GetSubmission(SID, Submission))
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Data"]["Result"] = (int)Submission.Result;
    ResponseJSON["Data"]["Description"] = Submission.Description;
    ResponseJSON["Data"]["PID"] = Submission.PID;
    ResponseJSON["Data"]["Code"] = Submission.Code;
    ResponseJSON["Data"]["Time"] = Submission.Time;
    ResponseJSON["Data"]["TimeSum"] = Submission.TimeSum;
    ResponseJSON["Data"]["Memory"] = Submission.Memory;
    ResponseJSON["Data"]["Score"] = Submission.Score;
    configor::json::array_type TestGroups;
    for (auto i : Submission.TestGroups)
    {
        configor::json TempTestGroup;
        TempTestGroup["TGID"] = i.TGID;
        TempTestGroup["Score"] = i.Score;
        TempTestGroup["Result"] = (int)i.Result;
        TempTestGroup["TestCasesPassed"] = i.TestCasesPassed;
        TempTestGroup["Time"] = i.Time;
        TempTestGroup["TimeSum"] = i.TimeSum;
        TempTestGroup["Memory"] = i.Memory;
        TempTestGroup["TestCases"].array({});
        configor::json::array_type TestCases;
        for (auto j : i.TestCases)
        {
            configor::json TempTestCase;
            TempTestCase["TCID"] = j.TCID;
            TempTestCase["Result"] = (int)j.Result;
            TempTestCase["Description"] = j.Description;
            TempTestCase["Time"] = j.Time;
            TempTestCase["TimeLimit"] = j.UnjudgedTestCase->TimeLimit;
            TempTestCase["Memory"] = j.Memory;
            TempTestCase["MemoryLimit"] = j.UnjudgedTestCase->MemoryLimit;
            TempTestCase["Score"] = j.Score;
            TestCases.push_back(TempTestCase);
        }
        TempTestGroup["TestCases"] = TestCases;
        TestGroups.push_back(TempTestGroup);
    }
    ResponseJSON["Data"]["TestGroups"] = TestGroups;
    return ResponseJSON;
}
configor::json WEB_DATA_PROCEED::Submit(std::string PID, bool EnableO2, std::string Code)
{
    SUBMISSION Submission;
    RETURN_JSON_IF_FAILED(Submission.Set(Code, PID))
    Submission.EnableO2 = EnableO2;
    RETURN_JSON_IF_FAILED(JudgingList.Add(Submission))
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Submit success";
    ResponseJSON["Data"]["SID"] = Submission.SID;
    return ResponseJSON;
}
configor::json WEB_DATA_PROCEED::GetProblem(std::string PID)
{
    configor::json ResponseJSON = BaseJSON;
    PROBLEM Problem;
    RETURN_JSON_IF_FAILED(PROBLEMS::GetProblem(PID, Problem))
    ResponseJSON["Success"] = true;
    ResponseJSON["Data"]["PID"] = Problem.PID;
    ResponseJSON["Data"]["Title"] = Problem.Title;
    ResponseJSON["Data"]["Description"] = Problem.Description;
    ResponseJSON["Data"]["Input"] = Problem.Input;
    ResponseJSON["Data"]["Output"] = Problem.Output;
    configor::json::array_type Samples;
    for (auto i : Problem.Samples)
    {
        configor::json TempSample;
        TempSample["Input"] = i.Input;
        TempSample["Output"] = i.Output;
        Samples.push_back(TempSample);
    }
    ResponseJSON["Data"]["Samples"] = Samples;
    configor::json::array_type TestGroups;
    for (auto i : Problem.TestGroups)
    {
        configor::json TempTestGroup;
        TempTestGroup["TGID"] = i.TGID;
        configor::json::array_type TestCases;
        for (auto j : i.TestCases)
        {
            configor::json TempTestCase;
            TempTestCase["TCID"] = j.TCID;
            TempTestCase["TimeLimit"] = j.TimeLimit;
            TempTestCase["MemoryLimit"] = j.MemoryLimit;
            TempTestCase["Score"] = j.Score;
            TestCases.push_back(TempTestCase);
        }
        TempTestGroup["TestCases"] = TestCases;
        TestGroups.push_back(TempTestGroup);
    }
    ResponseJSON["Data"]["TestGroups"] = TestGroups;
    ResponseJSON["Data"]["Range"] = Problem.Range;
    ResponseJSON["Data"]["Hint"] = Problem.Hint;
    ResponseJSON["Data"]["IOFilename"] = Problem.IOFilename;
    return ResponseJSON;
}

void WEB_DATA_PROCEED::TestAddProblem()
{
    PROBLEM TestProblem;
    std::vector<SAMPLE> SampleList;
    SAMPLE TestSample;
    TestSample.Input = "1 2";
    TestSample.Output = "3";
    SampleList.push_back(TestSample);
    TestProblem.PID = "1000";
    TestProblem.Title = "Add";
    TestProblem.Description = "Add two numbers";
    TestProblem.Input = "two numbers";
    TestProblem.Output = "one number";
    TestProblem.Samples = SampleList;
    TestProblem.Hint = "1<=a,b<=1000";
    TEST_GROUP_DATA TestGroup1;
    TestGroup1.AddTestCase("1 2", "3", 1000, 1024 * 1024 * 1024, 100);
    TestGroup1.AddTestCase("2 3", "5", 1000, 1024 * 1024 * 1024, 100);
    TestGroup1.AddTestCase("23 27", "50", 1000, 1024 * 1024 * 1024, 100);
    TestGroup1.AddTestCase("20 09", "29", 1000, 1024 * 1024 * 1024, 100);
    TestGroup1.AddTestCase("01 17", "18", 1000, 1024 * 1024 * 1024, 100);
    TestGroup1.AddTestCase("45 22", "67", 1000, 1024 * 1024 * 1024, 100);
    TestGroup1.AddTestCase("99 99", "198", 1000, 1024 * 1024 * 1024, 100);
    TestGroup1.AddTestCase("465 546", "1011", 1000, 1024 * 1024 * 1024, 100);
    TestGroup1.AddTestCase("123 456", "579", 1000, 1024 * 1024 * 1024, 100);
    TestGroup1.AddTestCase("877 479", "1356", 1000, 1024 * 1024 * 1024, 100);
    TestProblem.TestGroups.push_back(TestGroup1);
    TEST_GROUP_DATA TestGroup2;
    TestGroup2.AddTestCase("2147483648 2147483648", "4294967296", 1000, 1024 * 1024 * 1024, 100);
    TestGroup2.AddTestCase("6567867981 5617998756", "12185866737", 1000, 1024 * 1024 * 1024, 100);
    TestGroup2.AddTestCase("1234567890 9876543210", "11111111100", 1000, 1024 * 1024 * 1024, 100);
    TestGroup2.AddTestCase("1357924680 2468013579", "3825938259", 1000, 1024 * 1024 * 1024, 100);
    TestGroup2.AddTestCase("1122334455 6677889900", "7800224355", 1000, 1024 * 1024 * 1024, 100);
    TestProblem.TestGroups.push_back(TestGroup2);
    TEST_GROUP_DATA TestGroup3;
    TestGroup3.AddTestCase("1 -1", "0", 1000, 1024 * 1024 * 1024, 100);
    TestGroup3.AddTestCase("2 -3", "-1", 1000, 1024 * 1024 * 1024, 100);
    TestGroup3.AddTestCase("23 -27", "-4", 1000, 1024 * 1024 * 1024, 100);
    TestGroup3.AddTestCase("20 -09", "11", 1000, 1024 * 1024 * 1024, 100);
    TestGroup3.AddTestCase("01 -17", "-16", 1000, 1024 * 1024 * 1024, 100);
    TestGroup3.AddTestCase("45 -22", "23", 1000, 1024 * 1024 * 1024, 100);
    TestGroup3.AddTestCase("99 -99", "0", 1000, 1024 * 1024 * 1024, 100);
    TestGroup3.AddTestCase("465 -546", "-81", 1000, 1024 * 1024 * 1024, 100);
    TestGroup3.AddTestCase("123 -456", "-333", 1000, 1024 * 1024 * 1024, 100);
    TestGroup3.AddTestCase("877 -479", "398", 1000, 1024 * 1024 * 1024, 100);
    TestProblem.TestGroups.push_back(TestGroup3);
    TEST_GROUP_DATA TestGroup4;
    TestGroup4.AddTestCase("2147483647 -2147483647", "0", 1000, 1024 * 1024 * 1024, 100);
    TestGroup4.AddTestCase("6567867981 -5617998756", "949869225", 1000, 1024 * 1024 * 1024, 100);
    TestGroup4.AddTestCase("1234567890 -9876543210", "-8641975320", 1000, 1024 * 1024 * 1024, 100);
    TestGroup4.AddTestCase("1357924680 -2468013579", "-1110088899", 1000, 1024 * 1024 * 1024, 100);
    TestGroup4.AddTestCase("1122334455 -6677889900", "-5555555445", 1000, 1024 * 1024 * 1024, 100);
    TestProblem.TestGroups.push_back(TestGroup4);
    // UNJUDGED_TEST_GROUP TestGroup5; // WA
    // TestGroup5.AddTestCase("1 1", "3", 1000, 1024 * 1024 * 1024, 100);
    // TestProblem.TestGroups.push_back(TestGroup5);
    // UNJUDGED_TEST_GROUP TestGroup6; // TLE
    // TestGroup6.AddTestCase("1 2", "3", 1, 1024 * 1024 * 1024, 100);
    // TestProblem.TestGroups.push_back(TestGroup6);
    // UNJUDGED_TEST_GROUP TestGroup7; // MLE
    // TestGroup7.AddTestCase("1 2", "3", 1000, 1, 100);
    // TestProblem.TestGroups.push_back(TestGroup7);
    PROBLEMS::AddProblem(TestProblem);
}

HTTP_RESPONSE WEB_DATA_PROCEED::Proceed(HTTP_REQUEST HTTPRequest)
{
    std::string BasicFolder = "/home/langningc2009/Coding/Projects/OJ/HTML";
    HTTP_RESPONSE HTTPResponse;
    if (HTTPRequest.Path == "/api")
    {
        HTTPResponse.SetHeader("Content-Type", "application/json");
        configor::json ResponseJSON = BaseJSON;
        if (HTTPRequest.Verb != "POST")
            ResponseJSON["Message"] = "Only allow POST";
        else
        {
            try
            {
                configor::json RequestJSON = configor::json::parse(HTTPRequest.Body);
                if (RequestJSON["Action"].type() != configor::config_value_type::string)
                    ResponseJSON["Message"] = "Action must be string";
                else if (RequestJSON["Action"].as_string() == "Login")
                {
                    if (!CheckTypes(RequestJSON, {{"Username", configor::config_value_type::string},
                                                  {"Password", configor::config_value_type::string}}))
                        ResponseJSON["Message"] = "Invalid parameters";
                    else
                        ResponseJSON = Login(RequestJSON["Data"]["Username"].as_string(),
                                             RequestJSON["Data"]["Password"].as_string());
                }
                else if (RequestJSON["Action"].as_string() == "CheckUsernameAvailable")
                {
                    if (!CheckTypes(RequestJSON, {{"Username", configor::config_value_type::string}}))
                        ResponseJSON["Message"] = "Invalid parameters";
                    else
                        ResponseJSON = CheckUsernameAvailable(RequestJSON["Data"]["Username"].as_string());
                }
                else if (RequestJSON["Action"].as_string() == "SendVerifyCode")
                {
                    if (!CheckTypes(RequestJSON, {{"EmailAddress", configor::config_value_type::string}}))
                        ResponseJSON["Message"] = "Invalid parameters";
                    else
                        ResponseJSON = SendVerifyCode(RequestJSON["Data"]["EmailAddress"].as_string());
                }
                else if (RequestJSON["Action"].as_string() == "Register")
                {
                    if (!CheckTypes(RequestJSON, {{"Username", configor::config_value_type::string},
                                                  {"Password", configor::config_value_type::string},
                                                  {"EmailAddress", configor::config_value_type::string},
                                                  {"VerifyCode", configor::config_value_type::string}}))
                        ResponseJSON["Message"] = "Invalid parameters";
                    else
                        ResponseJSON = Register(RequestJSON["Data"]["Username"].as_string(),
                                                RequestJSON["Data"]["Password"].as_string(),
                                                RequestJSON["Data"]["EmailAddress"].as_string(),
                                                RequestJSON["Data"]["VerifyCode"].as_string());
                }
                else if (RequestJSON["Action"].as_string() == "CheckTokenAvailable")
                {
                    if (!CheckTypes(RequestJSON, {{"Token", configor::config_value_type::string}}))
                        ResponseJSON["Message"] = "Invalid parameters";
                    else
                        ResponseJSON = CheckTokenAvailable(RequestJSON["Data"]["Token"].as_string());
                }
                else if (RequestJSON["Action"].as_string() == "GetSubmission")
                {
                    if (!CheckTypes(RequestJSON, {{"SID", configor::config_value_type::number_integer},
                                                  {"Token", configor::config_value_type::string}}))
                        ResponseJSON["Message"] = "Invalid parameters";
                    else if (CheckTokenAvailable(RequestJSON["Data"]["Token"].as_string())["Success"].as_bool() == false)
                        ResponseJSON["Message"] = "Invalid token";
                    else
                        ResponseJSON = GetSubmission(RequestJSON["Data"]["SID"].as_integer());
                }
                else if (RequestJSON["Action"].as_string() == "Submit")
                {
                    if (!CheckTypes(RequestJSON, {{"PID", configor::config_value_type::string},
                                                  {"EnableO2", configor::config_value_type::boolean},
                                                  {"Code", configor::config_value_type::string},
                                                  {"Token", configor::config_value_type::string}}))
                        ResponseJSON["Message"] = "Invalid parameters";
                    else if (CheckTokenAvailable(RequestJSON["Data"]["Token"].as_string())["Success"].as_bool() == false)
                        ResponseJSON["Message"] = "Invalid token";
                    else
                        ResponseJSON = Submit(RequestJSON["Data"]["PID"].as_string(),
                                              RequestJSON["Data"]["EnableO2"].as_bool(),
                                              RequestJSON["Data"]["Code"].as_string());
                }
                else if (RequestJSON["Action"].as_string() == "GetProblem")
                {
                    if (!CheckTypes(RequestJSON, {{"PID", configor::config_value_type::string},
                                                  {"Token", configor::config_value_type::string}}))
                        ResponseJSON["Message"] = "Invalid parameters";
                    else if (CheckTokenAvailable(RequestJSON["Data"]["Token"].as_string())["Success"].as_bool() == false)
                        ResponseJSON["Message"] = "Invalid token";
                    else
                        ResponseJSON = GetProblem(RequestJSON["Data"]["PID"].as_string());
                }
                else
                    ResponseJSON["Message"] = "No such action";
            }
            catch (const configor::configor_exception &e)
            {
                ResponseJSON["Message"] = "JSON parse failed: "s + e.what();
            }
        }
        HTTPResponse.SetBody(ResponseJSON.dump());
    }
    else if (HTTPRequest.Path == "/main.js")
    {
        std::string Data;
        if (!UTILITIES::LoadFile(BasicFolder + "/main.js", Data).Success)
            Data = "System error: can not load file /main.js";
        HTTPResponse.SetBody(Data);
        HTTPResponse.SetHeader("Content-Type", "application/javascript");
    }
    else if (HTTPRequest.Path == "/main.css")
    {
        std::string Data;
        if (!UTILITIES::LoadFile(BasicFolder + "/main.css", Data).Success)
            Data = "System error: can not load file /main.css";
        HTTPResponse.SetBody(Data);
        HTTPResponse.SetHeader("Content-Type", "text/css");
    }
    else if (HTTPRequest.Path == "/TestAddProblem")
        TestAddProblem();
    else
    {
        std::string Data;
        if (!UTILITIES::LoadFile(BasicFolder + "/main.html", Data).Success)
            Data = "System error: can not load file /main.html";
        HTTPResponse.SetBody(Data);
    }
    return HTTPResponse;
}
