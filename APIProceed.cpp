#include "Result.hpp"
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
#include "EmailVerificationCodes.hpp"
#include "Submissions.hpp"
#include "APIProceed.hpp"
#include "WebDataProceed.hpp"

bool API_PROCEED::CheckTypes(configor::json JSON, std::vector<std::pair<std::string, configor::config_value_type>> Types)
{
    for (auto i : Types)
        if (JSON[i.first].type() != i.second)
            return false;
    return true;
}

configor::json API_PROCEED::CheckTokenAvailable(std::string Token)
{
    RETURN_JSON_IF_FAILED(TOKENS::CheckToken(Token))
    CREATE_JSON(true, "Token available");
}

configor::json API_PROCEED::Login(std::string Username, std::string Password)
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
configor::json API_PROCEED::CheckUsernameAvailable(std::string Username)
{
    RETURN_JSON_IF_FAILED(REGEXES::CheckUsername(Username))
    RETURN_JSON_IF_FAILED(USERS::CheckUsernameAvailable(Username))
    CREATE_JSON(true, "Username available");
}
configor::json API_PROCEED::CheckEmailAvailable(std::string EmailAddress)
{
    RETURN_JSON_IF_FAILED(REGEXES::CheckEmailAddress(EmailAddress))
    RETURN_JSON_IF_FAILED(USERS::CheckEmailAvailable(EmailAddress))
    CREATE_JSON(true, "Email available");
}
configor::json API_PROCEED::SendVerificationCode(std::string EmailAddress)
{
    RETURN_JSON_IF_FAILED(REGEXES::CheckEmailAddress(EmailAddress))
    std::string VerificationCode;
    RETURN_JSON_IF_FAILED(EMAIL_Verification_CODES::CreateEmailVerificationCode(EmailAddress, VerificationCode))
    RETURN_JSON_IF_FAILED(UTILITIES::SendEmail(EmailAddress, "Email verification Code", "Hello, here is your verification code. Your Verification code is " + VerificationCode + ". Thanks."))
    CREATE_JSON(true, "Send Verification code success")
}
configor::json API_PROCEED::Register(std::string Username, std::string Password, std::string EmailAddress, std::string VerificationCode)
{
    RETURN_JSON_IF_FAILED(REGEXES::CheckUsername(Username))
    RETURN_JSON_IF_FAILED(REGEXES::CheckPassword(Password))
    RETURN_JSON_IF_FAILED(REGEXES::CheckEmailAddress(EmailAddress))
    RETURN_JSON_IF_FAILED(REGEXES::CheckVerificationCode(VerificationCode))
    RETURN_JSON_IF_FAILED(EMAIL_Verification_CODES::CheckEmailVerificationCode(EmailAddress, VerificationCode))
    RETURN_JSON_IF_FAILED(EMAIL_Verification_CODES::DeleteEmailVerificationCode(EmailAddress))
    RETURN_JSON_IF_FAILED(USERS::CheckUsernameAvailable(Username))
    RETURN_JSON_IF_FAILED(USERS::CheckEmailAvailable(EmailAddress))
    RETURN_JSON_IF_FAILED(USERS::CreateUser(Username, Password, EmailAddress, ""))
    CREATE_JSON(true, "Register success");
}

configor::json API_PROCEED::AddProblem(std::string PID, std::string Title, std::string IOFilename, std::string Description, std::string Input, std::string Output, std::string Range, std::string Hint, std::string Samples, std::string TestGroups)
{
    if (!IsAdmin)
        CREATE_JSON(false, "Not admin");
    PROBLEM Problem;
    Problem.PID = PID;
    Problem.Title = Title;
    Problem.IOFilename = IOFilename;
    Problem.Description = Description;
    Problem.Input = Input;
    Problem.Output = Output;
    Problem.Range = Range;
    Problem.Hint = Hint;
    RETURN_JSON_IF_FAILED(PROBLEMS::JSONToSamples(Samples, Problem.Samples))
    RETURN_JSON_IF_FAILED(PROBLEMS::JSONToUnjudgedTestGroups(TestGroups, Problem.TestGroups))
    RETURN_JSON_IF_FAILED(PROBLEMS::AddProblem(Problem))
    CREATE_JSON(true, "Add problem success");
}
configor::json API_PROCEED::GetProblem(std::string PID)
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
        TempSample["Description"] = i.Description;
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
            if (IsAdmin)
            {
                TempTestCase["Input"] = j.Input;
                TempTestCase["Answer"] = j.Answer;
            }
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
configor::json API_PROCEED::UpdateProblem(std::string PID, std::string Title, std::string IOFilename, std::string Description, std::string Input, std::string Output, std::string Range, std::string Hint, std::string Samples, std::string TestGroups)
{
    if (!IsAdmin)
        CREATE_JSON(false, "Not admin");
    PROBLEM Problem;
    Problem.PID = PID;
    Problem.Title = Title;
    Problem.IOFilename = IOFilename;
    Problem.Description = Description;
    Problem.Input = Input;
    Problem.Output = Output;
    Problem.Range = Range;
    Problem.Hint = Hint;
    RETURN_JSON_IF_FAILED(PROBLEMS::JSONToSamples(Samples, Problem.Samples))
    RETURN_JSON_IF_FAILED(PROBLEMS::JSONToUnjudgedTestGroups(TestGroups, Problem.TestGroups))
    RETURN_JSON_IF_FAILED(PROBLEMS::UpdateProblem(Problem))
    CREATE_JSON(true, "Update problem success");
}
configor::json API_PROCEED::DeleteProblem(std::string PID)
{
    if (!IsAdmin)
        CREATE_JSON(false, "Not admin");
    RETURN_JSON_IF_FAILED(PROBLEMS::DeleteProblem(PID))
    CREATE_JSON(true, "Delete problem success");
}
configor::json API_PROCEED::GetProblems(int Page)
{
    configor::json ResponseJSON = BaseJSON;
    RETURN_JSON_IF_FAILED(DATABASE::SELECT("Problems")
                              .Select("PID")
                              .Select("Title")
                              .Limit(10)
                              .Offset((Page - 1) * 10)
                              .Execute(
                                  [&ResponseJSON](auto Data)
                                  {
                                      ResponseJSON["Success"] = true;
                                      configor::json::array_type Problems;
                                      for (auto i : Data)
                                      {
                                          configor::json TempProblem;
                                          TempProblem["PID"] = i["PID"];
                                          TempProblem["Title"] = i["Title"];
                                          Problems.push_back(TempProblem);
                                      }
                                      ResponseJSON["Data"]["Problems"] = Problems;
                                      CREATE_RESULT(true, "Get problems success");
                                  }));
    RETURN_JSON_IF_FAILED(DATABASE::SIZE("Problems")
                              .Execute(
                                  [&ResponseJSON](int Size)
                                  {
                                      ResponseJSON["Data"]["PageCount"] = ceil(Size / 10.0);
                                  }));
    return ResponseJSON;
}

configor::json API_PROCEED::AddSubmission(std::string PID, bool EnableO2, std::string Code)
{
    SUBMISSION Submission;
    RETURN_JSON_IF_FAILED(Submission.Set(Code, PID))
    Submission.EnableO2 = EnableO2;
    RETURN_JSON_IF_FAILED(SUBMISSIONS::AddSubmission(Submission))
    RETURN_JSON_IF_FAILED(JudgingList.Add(Submission))
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Add submission success";
    ResponseJSON["Data"]["SID"] = Submission.SID;
    return ResponseJSON;
}
configor::json API_PROCEED::GetSubmission(int SID)
{
    SUBMISSION Submission;
    RETURN_JSON_IF_FAILED(SUBMISSIONS::GetSubmission(SID, Submission))
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Data"]["Result"] = (int)Submission.Result;
    ResponseJSON["Data"]["Description"] = Submission.Description;
    ResponseJSON["Data"]["PID"] = Submission.PID;
    if (IsAdmin || Submission.UID == UID)
    {
        ResponseJSON["Data"]["Code"] = Submission.Code;
    }
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
configor::json API_PROCEED::UpdateSubmission(int SID, std::string PID, int UID, std::string Code, int Result, std::string Description, int Time, int TimeSum, int Memory, int Score, bool EnableO2, std::string TestGroups)
{
    if (!IsAdmin)
        CREATE_JSON(false, "Not admin");
    SUBMISSION Submission;
    Submission.PID = PID;
    Submission.UID = UID;
    Submission.Code = Code;
    Submission.Result = (JUDGE_RESULT)Result;
    Submission.Description = Description;
    Submission.Time = Time;
    Submission.TimeSum = TimeSum;
    Submission.Memory = Memory;
    Submission.Score = Score;
    Submission.EnableO2 = EnableO2;
    RETURN_JSON_IF_FAILED(SUBMISSIONS::JSONToTestGroups(TestGroups, Submission.TestGroups, PID, SID));
    RETURN_JSON_IF_FAILED(SUBMISSIONS::UpdateSubmission(Submission))
    CREATE_JSON(true, "Update problem success");
}
configor::json API_PROCEED::RejudgeSubmission(int SID)
{
    if (!IsAdmin)
        CREATE_JSON(false, "Not admin");
    SUBMISSION Submission;
    RETURN_JSON_IF_FAILED(SUBMISSIONS::GetSubmission(SID, Submission));
    Submission.Result = JUDGE_RESULT::WAITING;
    Submission.Time = Submission.TimeSum = Submission.Memory = Submission.Score = 0;
    for (auto &i : Submission.TestGroups)
    {
        i.Result = JUDGE_RESULT::WAITING;
        i.Time = i.TimeSum = i.Memory = i.TestCasesPassed = i.Score = 0;
        for (auto &j : i.TestCases)
        {
            j.Result = JUDGE_RESULT::WAITING;
            j.Output = j.StandardOutput = j.StandardError = j.Description = "";
            j.Time = j.Memory = j.Score = 0; // TODO: Score here is not correct
        }
    }
    RETURN_JSON_IF_FAILED(SUBMISSIONS::UpdateSubmission(Submission));
    RETURN_JSON_IF_FAILED(JudgingList.Add(Submission));
    CREATE_JSON(true, "Rejudge submission success");
}
configor::json API_PROCEED::DeleteSubmission(int SID)
{
    if (!IsAdmin)
        CREATE_JSON(false, "Not admin");
    RETURN_JSON_IF_FAILED(SUBMISSIONS::DeleteSubmission(SID))
    CREATE_JSON(true, "Delete submission success");
}
configor::json API_PROCEED::GetSubmissions(int Page)
{
    configor::json ResponseJSON = BaseJSON;
    RETURN_JSON_IF_FAILED(DATABASE::SELECT("Submissions")
                              .Select("SID")
                              .Select("PID")
                              .Select("UID")
                              .Select("Result")
                              .Select("Time")
                              .Select("Memory")
                              .Select("CreateTime")
                              .Order("SID", false)
                              .Offset((Page - 1) * 10)
                              .Limit(10)
                              .Execute(
                                  [&ResponseJSON](auto Data)
                                  {
                                      ResponseJSON["Success"] = true;
                                      configor::json::array_type Submissions;
                                      for (auto i : Data)
                                      {
                                          configor::json TempSubmission;
                                          TempSubmission["SID"] = i["SID"];
                                          TempSubmission["PID"] = i["PID"];
                                          TempSubmission["UID"] = i["UID"];
                                          TempSubmission["Result"] = i["Result"];
                                          TempSubmission["Time"] = i["Time"];
                                          TempSubmission["Memory"] = i["Memory"];
                                          TempSubmission["CreateTime"] = i["CreateTime"];
                                          Submissions.push_back(TempSubmission);
                                      }
                                      ResponseJSON["Data"]["Submissions"] = Submissions;
                                      CREATE_RESULT(true, "Get submissions success");
                                  }));
    RETURN_JSON_IF_FAILED(DATABASE::SIZE("Submissions")
                              .Execute(
                                  [&ResponseJSON](int Size)
                                  {
                                      ResponseJSON["Data"]["PageCount"] = ceil(Size / 10.0);
                                  }));
    return ResponseJSON;
}

configor::json API_PROCEED::GetSettings()
{
    if (!IsAdmin)
        CREATE_JSON(false, "Not admin");
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    RETURN_JSON_IF_FAILED(SETTINGS::GetSettings(ResponseJSON["Data"]["Settings"]));
    return ResponseJSON;
}
configor::json API_PROCEED::SetSettings(configor::json Settings)
{
    if (!IsAdmin)
        CREATE_JSON(false, "Not admin");
    RETURN_JSON_IF_FAILED(SETTINGS::SetSettings(Settings));
    CREATE_JSON(true, "Set settings success");
}

void API_PROCEED::TestAddProblem()
{
    PROBLEM TestProblem;
    std::vector<SAMPLE> SampleList;
    SAMPLE TestSample;
    TestSample.Input = "1 2";
    TestSample.Output = "3";
    SampleList.push_back(TestSample);
    TestProblem.PID = "1000";
    TestProblem.Title = "Add";
    TestProblem.Description = "**Add** two numbers $a$ and $b$";
    TestProblem.Input = "two numbers in _one line_";
    TestProblem.Output = "one number";
    TestProblem.Samples = SampleList;
    TestProblem.Hint = "$$1<=a,b<=1000$$";
    TEST_GROUP_DATA TestGroup1;
    TestGroup1.TGID = 0;
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
    TestGroup2.TGID = 1;
    TestGroup2.AddTestCase("2147483648 2147483648", "4294967296", 1000, 1024 * 1024 * 1024, 100);
    TestGroup2.AddTestCase("6567867981 5617998756", "12185866737", 1000, 1024 * 1024 * 1024, 100);
    TestGroup2.AddTestCase("1234567890 9876543210", "11111111100", 1000, 1024 * 1024 * 1024, 100);
    TestGroup2.AddTestCase("1357924680 2468013579", "3825938259", 1000, 1024 * 1024 * 1024, 100);
    TestGroup2.AddTestCase("1122334455 6677889900", "7800224355", 1000, 1024 * 1024 * 1024, 100);
    TestProblem.TestGroups.push_back(TestGroup2);
    TEST_GROUP_DATA TestGroup3;
    TestGroup3.TGID = 2;
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
    TestGroup4.TGID = 3;
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
configor::json API_PROCEED::Proceed(configor::json Request)
{
    configor::json ResponseJSON = BaseJSON;
    if (!CheckTypes(Request, {{"Action", configor::config_value_type::string}}))
        ResponseJSON["Message"] = "Invalid parameters";
    else
    {
        Action = Request["Action"].as_string();
        Data = Request["Data"];
        if (Action == "CheckTokenAvailable")
        {
            if (!CheckTypes(Data, {{"Token", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = CheckTokenAvailable(Data["Token"].as_string());
        }
        else if (Action == "Register")
        {
            if (!CheckTypes(Data, {{"Username", configor::config_value_type::string},
                                   {"Password", configor::config_value_type::string},
                                   {"EmailAddress", configor::config_value_type::string},
                                   {"VerificationCode", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = Register(Data["Username"].as_string(),
                                        Data["Password"].as_string(),
                                        Data["EmailAddress"].as_string(),
                                        Data["VerificationCode"].as_string());
        }
        else if (Action == "CheckUsernameAvailable")
        {
            if (!CheckTypes(Data, {{"Username", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = CheckUsernameAvailable(Data["Username"].as_string());
        }
        else if (Action == "CheckEmailAvailable")
        {
            if (!CheckTypes(Data, {{"EmailAddress", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = CheckEmailAvailable(Data["EmailAddress"].as_string());
        }
        else if (Action == "SendVerificationCode")
        {
            if (!CheckTypes(Data, {{"EmailAddress", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = SendVerificationCode(Data["EmailAddress"].as_string());
        }
        else if (Action == "Login")
        {
            if (!CheckTypes(Data, {{"Username", configor::config_value_type::string},
                                   {"Password", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = Login(Data["Username"].as_string(),
                                     Data["Password"].as_string());
        }
        else
        {
            if (!CheckTypes(Data, {{"Token", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
            {
                Token = Data["Token"].as_string();
                if (CheckTokenAvailable(Token)["Success"].as_bool() == false)
                    ResponseJSON["Message"] = "Invalid token";
                else
                {
                    RETURN_JSON_IF_FAILED(TOKENS::GetUID(Token, UID));
                    RETURN_JSON_IF_FAILED(USERS::IsAdmin(UID, IsAdmin));

                    if (Action == "AddProblem")
                    {
                        if (!CheckTypes(Data, {{"PID", configor::config_value_type::string},
                                               {"Title", configor::config_value_type::string},
                                               {"IOFilename", configor::config_value_type::string},
                                               {"Description", configor::config_value_type::string},
                                               {"Input", configor::config_value_type::string},
                                               {"Output", configor::config_value_type::string},
                                               {"Range", configor::config_value_type::string},
                                               {"Hint", configor::config_value_type::string},
                                               {"Samples", configor::config_value_type::string},
                                               {"TestGroups", configor::config_value_type::string}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = AddProblem(Data["PID"].as_string(),
                                                      Data["Title"].as_string(),
                                                      Data["IOFilename"].as_string(),
                                                      Data["Description"].as_string(),
                                                      Data["Input"].as_string(),
                                                      Data["Output"].as_string(),
                                                      Data["Range"].as_string(),
                                                      Data["Hint"].as_string(),
                                                      Data["Samples"].as_string(),
                                                      Data["TestGroups"].as_string());
                    }
                    else if (Action == "GetProblem")
                    {
                        if (!CheckTypes(Data, {{"PID", configor::config_value_type::string}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = GetProblem(Data["PID"].as_string());
                    }
                    else if (Action == "UpdateProblem")
                    {
                        if (!CheckTypes(Data, {{"PID", configor::config_value_type::string},
                                               {"Title", configor::config_value_type::string},
                                               {"IOFilename", configor::config_value_type::string},
                                               {"Description", configor::config_value_type::string},
                                               {"Input", configor::config_value_type::string},
                                               {"Output", configor::config_value_type::string},
                                               {"Range", configor::config_value_type::string},
                                               {"Hint", configor::config_value_type::string},
                                               {"Samples", configor::config_value_type::string},
                                               {"TestGroups", configor::config_value_type::string}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = UpdateProblem(Data["PID"].as_string(),
                                                         Data["Title"].as_string(),
                                                         Data["IOFilename"].as_string(),
                                                         Data["Description"].as_string(),
                                                         Data["Input"].as_string(),
                                                         Data["Output"].as_string(),
                                                         Data["Range"].as_string(),
                                                         Data["Hint"].as_string(),
                                                         Data["Samples"].as_string(),
                                                         Data["TestGroups"].as_string());
                    }
                    else if (Action == "DeleteProblem")
                    {
                        if (!CheckTypes(Data, {{"PID", configor::config_value_type::string}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = DeleteProblem(Data["PID"].as_string());
                    }
                    else if (Action == "GetProblems")
                    {
                        if (!CheckTypes(Data, {{"Page", configor::config_value_type::number_integer}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = GetProblems(Data["Page"].as_integer());
                    }
                    else if (Action == "AddSubmission")
                    {
                        if (!CheckTypes(Data, {{"PID", configor::config_value_type::string},
                                               {"EnableO2", configor::config_value_type::boolean},
                                               {"Code", configor::config_value_type::string}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = AddSubmission(Data["PID"].as_string(),
                                                         Data["EnableO2"].as_bool(),
                                                         Data["Code"].as_string());
                    }
                    else if (Action == "GetSubmission")
                    {
                        if (!CheckTypes(Data, {{"SID", configor::config_value_type::number_integer}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = GetSubmission(Data["SID"].as_integer());
                    }
                    else if (Action == "UpdateSubmission")
                    {
                        if (!CheckTypes(Data, {{"SID", configor::config_value_type::number_integer},
                                               {"PID", configor::config_value_type::string},
                                               {"UID", configor::config_value_type::number_integer},
                                               {"Code", configor::config_value_type::string},
                                               {"Result", configor::config_value_type::number_integer},
                                               {"Description", configor::config_value_type::string},
                                               {"Time", configor::config_value_type::number_integer},
                                               {"TimeSum", configor::config_value_type::number_integer},
                                               {"Memory", configor::config_value_type::number_integer},
                                               {"Score", configor::config_value_type::number_integer},
                                               {"EnableO2", configor::config_value_type::boolean},
                                               {"TestGroups", configor::config_value_type::string}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = UpdateSubmission(Data["SID"].as_integer(),
                                                            Data["PID"].as_string(),
                                                            Data["UID"].as_integer(),
                                                            Data["Code"].as_string(),
                                                            Data["Result"].as_integer(),
                                                            Data["Description"].as_string(),
                                                            Data["Time"].as_integer(),
                                                            Data["TimeSum"].as_integer(),
                                                            Data["Memory"].as_integer(),
                                                            Data["Score"].as_integer(),
                                                            Data["EnableO2"].as_bool(),
                                                            Data["TestGroups"].as_string());
                    }
                    else if (Action == "RejudgeSubmission")
                    {
                        if (!CheckTypes(Data, {{"SID", configor::config_value_type::number_integer}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = RejudgeSubmission(Data["SID"].as_integer());
                    }
                    else if (Action == "DeleteSubmission")
                    {
                        if (!CheckTypes(Data, {{"SID", configor::config_value_type::number_integer}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = DeleteSubmission(Data["SID"].as_integer());
                    }
                    else if (Action == "GetSubmissions")
                    {
                        if (!CheckTypes(Data, {{"Page", configor::config_value_type::number_integer}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = GetSubmissions(Data["Page"].as_integer());
                    }
                    else if (Action == "GetSettings")
                    {
                        ResponseJSON = GetSettings();
                    }
                    else if (Action == "SetSettings")
                    {
                        if (!CheckTypes(Data, {{"Settings", configor::config_value_type::object}}))
                            ResponseJSON["Message"] = "Invalid parameters";
                        else
                            ResponseJSON = SetSettings(Data["Settings"]);
                    }
                    else
                        ResponseJSON["Message"] = "No such action";
                    ResponseJSON["Data"]["IsAdmin"] = IsAdmin;
                }
            }
        }
    }
    return ResponseJSON;
}
