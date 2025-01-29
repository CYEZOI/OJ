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

#include "APIProceed.hpp"
#include "EmailVerificationCodes.hpp"
#include "Exception.hpp"
#include "JudgingList.hpp"
#include "Passkey.hpp"
#include "Problems.hpp"
#include "Regexes.hpp"
#include "Settings.hpp"
#include "Submission.hpp"
#include "Submissions.hpp"
#include "TestCase.hpp"
#include "TestGroup.hpp"
#include "Tokens.hpp"
#include "Users.hpp"
#include "Utilities.hpp"
#include "WebDataProceed.hpp"
#include <minizip/unzip.h>

bool API_PROCEED::CheckTypes(configor::json JSON, std::vector<std::pair<std::string, configor::config_value_type>> Types) {
    for (auto i : Types)
        if (JSON[i.first].type() != i.second)
            return false;
    return true;
}

configor::json API_PROCEED::CheckTokenAvailable(std::string Token) {
    TOKENS::CheckToken(Token);
    RETURN_SUCCESS("Token available");
}

configor::json API_PROCEED::Login(std::string Username, std::string Password) {
    REGEXES::CheckUsername(Username);
    REGEXES::CheckPassword(Password);
    int UID;
    USERS::CheckPasswordCorrect(Username, USERS::HashPassword(Password), UID);
    std::string Token = TOKENS::CreateToken(UID);
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Login succeeds";
    ResponseJSON["Data"]["Token"] = Token;
    ResponseJSON["Data"]["IsAdmin"] = USERS::IsAdmin(UID);
    ResponseJSON["Data"]["UID"] = UID;
    return ResponseJSON;
}
configor::json API_PROCEED::CheckUsernameAvailable(std::string Username) {
    REGEXES::CheckUsername(Username);
    USERS::CheckUsernameAvailable(Username);
    RETURN_SUCCESS("Username available");
}
configor::json API_PROCEED::CheckEmailAvailable(std::string EmailAddress) {
    REGEXES::CheckEmailAddress(EmailAddress);
    USERS::CheckEmailAvailable(EmailAddress);
    RETURN_SUCCESS("Email available");
}
configor::json API_PROCEED::SendVerificationCode(std::string EmailAddress) {
    REGEXES::CheckEmailAddress(EmailAddress);
    std::string VerificationCode;
    EMAIL_VERIFICATION_CODES::CreateEmailVerificationCode(EmailAddress, VerificationCode);
    UTILITIES::SendEmail(EmailAddress, "Email verification Code", "Hello, here is your verification code. Your Verification code is " + VerificationCode + ". Thanks.");
    RETURN_SUCCESS("Send verification code succeeds")
}
configor::json API_PROCEED::Register(std::string Username, std::string Nickname, std::string Password, std::string EmailAddress, std::string VerificationCode) {
    REGEXES::CheckUsername(Username);
    REGEXES::CheckNickname(Nickname);
    REGEXES::CheckPassword(Password);
    REGEXES::CheckEmailAddress(EmailAddress);
    REGEXES::CheckVerificationCode(VerificationCode);
    EMAIL_VERIFICATION_CODES::CheckEmailVerificationCode(EmailAddress, VerificationCode);
    EMAIL_VERIFICATION_CODES::DeleteEmailVerificationCode(EmailAddress);
    USERS::CheckUsernameAvailable(Username);
    USERS::CheckEmailAvailable(EmailAddress);
    USERS::AddUser(Username, Nickname, USERS::HashPassword(Password), EmailAddress, USER_ROLE::USER_ROLE_USER);
    RETURN_SUCCESS("Register succeeds");
}
configor::json API_PROCEED::ResetPassword(std::string EmailAddress, std::string VerificationCode, std::string Password) {
    REGEXES::CheckEmailAddress(EmailAddress);
    REGEXES::CheckVerificationCode(VerificationCode);
    EMAIL_VERIFICATION_CODES::CheckEmailVerificationCode(EmailAddress, VerificationCode);
    EMAIL_VERIFICATION_CODES::DeleteEmailVerificationCode(EmailAddress);
    USERS::UpdateUserPassword(USERS::GetUIDByEmailAddress(EmailAddress), USERS::HashPassword(Password));
    RETURN_SUCCESS("Reset password succeeds");
}

configor::json API_PROCEED::CreatePasskeyChallenge() {
    std::string ChallengeChallengeID = PASSKEY::CreateChallenge();
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Create passkey challenge succeeds";
    ResponseJSON["Data"]["Challenge"] = ChallengeChallengeID;
    return ResponseJSON;
}
configor::json API_PROCEED::DeletePasskeyChallenge(std::string Challenge) {
    PASSKEY::DeleteChallenge(Challenge);
    RETURN_SUCCESS("Delete passkey challenge succeeds");
}
configor::json API_PROCEED::CreatePasskey(std::string Challenge, std::string CredentialID, std::string CredentialPublicKey) {
    PASSKEY::CreatePasskey(UID, Challenge, CredentialID, CredentialPublicKey);
    RETURN_SUCCESS("Create passkey succeeds");
}
configor::json API_PROCEED::LoginWithPasskey(std::string Challenge, std::string CredentialID, int UserHandle, std::string CredentialSignature) {
    std::string PublicKey = PASSKEY::GetPasskey(UserHandle, CredentialID);
    if (PublicKey == "")
        throw EXCEPTION("Invalid credential");
    DATABASE::SELECT("PasskeyChallenges")
        .Select("CreateTime")
        .Where("Challenge", Challenge)
        .Execute(
            [Challenge](auto Data) {
                if (Data.size() != 1)
                    throw EXCEPTION("Invalid challenge");
                DATABASE::DELETE("PasskeyChallenges")
                    .Where("Challenge", Challenge)
                    .Execute();
                if (UTILITIES::StringToTime(Data[0]["CreateTime"]) + 60 < time(NULL))
                    throw EXCEPTION("Challenge expired");
            });

    if (!UTILITIES::VerifySignature(CredentialSignature, CredentialID, PublicKey))
        throw EXCEPTION("Invalid credential signature");
    std::string Token = TOKENS::CreateToken(UserHandle);
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Login with passkey succeeds";
    ResponseJSON["Data"]["Token"] = Token;
    ResponseJSON["Data"]["IsAdmin"] = USERS::IsAdmin(UserHandle);
    ResponseJSON["Data"]["UID"] = UserHandle;
    ResponseJSON["Data"]["Username"] = USERS::GetUser(UserHandle).Username;
    return ResponseJSON;
}

configor::json API_PROCEED::AddUser(std::string Username, std::string Nickname, std::string Password, std::string EmailAddress, USER_ROLE Role) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    USERS::AddUser(Username, Nickname, USERS::HashPassword(Password), EmailAddress, Role);
    RETURN_SUCCESS("Add user succeeds");
}
configor::json API_PROCEED::UpdateUser(int UID, std::string Username, std::string Nickname, std::string HashedPassword, std::string EmailAddress, USER_ROLE Role) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    REGEXES::CheckUsername(Username);
    REGEXES::CheckNickname(Nickname);
    REGEXES::CheckEmailAddress(EmailAddress);
    USER OriginalUser = USERS::GetUser(UID);
    if (OriginalUser.Username != Username)
        USERS::CheckUsernameAvailable(Username);
    if (OriginalUser.EmailAddress != EmailAddress)
        USERS::CheckEmailAvailable(EmailAddress);
    USERS::UpdateUser(UID, Username, Nickname, HashedPassword, EmailAddress, Role);
    RETURN_SUCCESS("Update user succeeds");
}
configor::json API_PROCEED::DeleteUser(int UID) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    USERS::DeleteUser(UID);
    RETURN_SUCCESS("Delete user succeeds");
}
configor::json API_PROCEED::GetUser(int UID) {
    configor::json ResponseJSON = BaseJSON;
    USER User;
    User = USERS::GetUser(UID);
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Get user succeeds";
    ResponseJSON["Data"]["Username"] = User.Username;
    ResponseJSON["Data"]["EmailAddress"] = User.EmailAddress;
    ResponseJSON["Data"]["Nickname"] = User.Nickname;
    ResponseJSON["Data"]["Role"] = User.Role;
    return ResponseJSON;
}
configor::json API_PROCEED::GetUsers(int Page) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    configor::json ResponseJSON = BaseJSON;
    DATABASE::SELECT("Users")
        .Select("UID")
        .Select("Username")
        .Select("Password")
        .Select("Nickname")
        .Select("EmailAddress")
        .Select("Role")
        .Limit(10)
        .Offset((Page - 1) * 10)
        .Execute(
            [&ResponseJSON](auto Data) {
                ResponseJSON["Success"] = true;
                ResponseJSON["Message"] = "Get users succeeds";
                configor::json::array_type Users;
                for (auto i : Data) {
                    configor::json TempUser;
                    TempUser["UID"] = i["UID"];
                    TempUser["Username"] = i["Username"];
                    TempUser["Nickname"] = i["Nickname"];
                    TempUser["EmailAddress"] = i["EmailAddress"];
                    TempUser["Password"] = i["Password"];
                    TempUser["Role"] = i["Role"];
                    Users.push_back(TempUser);
                }
                ResponseJSON["Data"]["Users"] = Users;
            });
    DATABASE::SIZE("Users")
        .Execute(
            [&ResponseJSON](int Size) {
                ResponseJSON["Data"]["PageCount"] = ceil(Size / 10.0);
            });
    return ResponseJSON;
}
configor::json API_PROCEED::HashPassword(std::string OriginalPassword) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Hash password succeeds";
    ResponseJSON["Data"]["HashedPassword"] = USERS::HashPassword(OriginalPassword);
    return ResponseJSON;
}

configor::json API_PROCEED::AddProblem(std::string PID, std::string Title, std::string IOFilename, std::string Description, std::string Input, std::string Output, std::string Range, std::string Hint, std::string Samples, std::string TestGroups) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    PROBLEM Problem;
    Problem.PID = PID;
    Problem.Title = Title;
    Problem.IOFilename = IOFilename;
    Problem.Description = Description;
    Problem.Input = Input;
    Problem.Output = Output;
    Problem.Range = Range;
    Problem.Hint = Hint;
    PROBLEMS::JSONToSamples(Samples, Problem.Samples);
    PROBLEMS::JSONToUnjudgedTestGroups(TestGroups, Problem.TestGroups);
    PROBLEMS::AddProblem(Problem);
    RETURN_SUCCESS("Add problem succeeds");
}
configor::json API_PROCEED::GetProblem(std::string PID) {
    configor::json ResponseJSON = BaseJSON;
    PROBLEM Problem;
    PROBLEMS::GetProblem(PID, Problem);
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Get problem succeeds";
    ResponseJSON["Data"]["PID"] = Problem.PID;
    ResponseJSON["Data"]["Title"] = Problem.Title;
    ResponseJSON["Data"]["Description"] = Problem.Description;
    ResponseJSON["Data"]["Input"] = Problem.Input;
    ResponseJSON["Data"]["Output"] = Problem.Output;
    configor::json::array_type Samples;
    for (auto i : Problem.Samples) {
        configor::json TempSample;
        TempSample["Input"] = i.Input;
        TempSample["Output"] = i.Output;
        TempSample["Description"] = i.Description;
        Samples.push_back(TempSample);
    }
    ResponseJSON["Data"]["Samples"] = Samples;
    configor::json::array_type TestGroups;
    for (auto i : Problem.TestGroups) {
        configor::json TempTestGroup;
        TempTestGroup["TGID"] = i.TGID;
        configor::json::array_type TestCases;
        for (auto j : i.TestCases) {
            configor::json TempTestCase;
            TempTestCase["TCID"] = j.TCID;
            TempTestCase["TimeLimit"] = j.TimeLimit;
            TempTestCase["MemoryLimit"] = j.MemoryLimit;
            TempTestCase["Score"] = j.Score;
            if (IsAdmin) {
                TempTestCase["InputFilename"] = j.InputFilename;
                TempTestCase["AnswerFilename"] = j.AnswerFilename;
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
configor::json API_PROCEED::UploadTestCase(std::string PID, std::string Data) {
    if (Data.find(",") == std::string::npos)
        throw EXCEPTION("Data not available");
    Data = Data.substr(Data.find(",") + 1);

    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    int in_len = Data.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string Result;

    while (in_len-- && (Data[in_] != '=')) {
        char_array_4[i++] = Data[in_];
        in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                Result += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            Result += char_array_3[j];
    }

    std::string ZipFilename = "/tmp/" + PID + ".zip";
    FILE *FilePointer = fopen(ZipFilename.c_str(), "w");
    if (FilePointer == NULL) throw EXCEPTION("Can not open zip file");
    if (fwrite(Result.c_str(), 1, Result.size(), FilePointer) != Result.size())
        throw EXCEPTION("Can not write to zip file");
    fclose(FilePointer);

    std::string JudgeUsername;
    SETTINGS::GetSettings("JudgeUsername", JudgeUsername);
    std::string IODataDir = "/home/" + JudgeUsername + "/IOData/" + PID;
    UTILITIES::MakeDir(IODataDir);
    UTILITIES::RemoveDir(IODataDir);
    UTILITIES::MakeDir(IODataDir);

    std::vector<std::string> InputFilenames;
    std::vector<std::string> AnswerFilenames;
    unzFile ZipFile = unzOpen(ZipFilename.c_str());
    if (!ZipFile) throw EXCEPTION("Can not open zip file");
    int ZipStatus = unzGoToFirstFile(ZipFile);
    while (true) {
        char FilenameBuffer[256];
        memset(FilenameBuffer, 0, 256 * sizeof(char));
        unz_file_info FileInfo;
        if (unzGetCurrentFileInfo(ZipFile, &FileInfo, FilenameBuffer, sizeof(FilenameBuffer), NULL, 0, NULL, 0) != UNZ_OK) {
            unzClose(ZipFile);
            throw EXCEPTION("Unzip file get info failed");
        }
        std::string Filename = FilenameBuffer;
        if (Filename.find_last_of("/") != std::string::npos)
            Filename = Filename.substr(Filename.find_last_of("/") + 1);
        if (Filename.find(".in") != std::string::npos)
            InputFilenames.push_back(Filename);
        else if (Filename.find(".out") != std::string::npos || Filename.find(".ans") != std::string::npos)
            AnswerFilenames.push_back(Filename);
        else {
            unzClose(ZipFile);
            throw EXCEPTION("Filename " + Filename + " not recognized");
        }
        if (unzOpenCurrentFile(ZipFile) != UNZ_OK) {
            unzClose(ZipFile);
            throw EXCEPTION("Unzip file open file failed");
        }
        char *FileDataBuffer = new char[FileInfo.uncompressed_size];
        memset(FileDataBuffer, 0, FileInfo.uncompressed_size * sizeof(char));
        if (unzReadCurrentFile(ZipFile, FileDataBuffer, FileInfo.uncompressed_size) < 0) {
            unzCloseCurrentFile(ZipFile);
        }
        std::string FullFilePath = IODataDir + "/" + Filename;
        FILE *FilePointer = fopen(FullFilePath.c_str(), "w");
        if (FilePointer == NULL)
            throw EXCEPTION("Can not open output file " + FullFilePath);
        if (fwrite(FileDataBuffer, 1, FileInfo.uncompressed_size, FilePointer) != FileInfo.uncompressed_size) {
            fclose(FilePointer);
            unzClose(ZipFile);
            throw EXCEPTION("Can not write to file " + FullFilePath);
        }
        delete[] FileDataBuffer;
        fclose(FilePointer);
        unzCloseCurrentFile(ZipFile);
        ZipStatus = unzGoToNextFile(ZipFile);
        if (ZipStatus == UNZ_END_OF_LIST_OF_FILE) {
            break;
        } else if (ZipStatus != UNZ_OK) {
            throw EXCEPTION("Can not read zip file " + ZipFilename);
        }
    }
    unzClose(ZipFile);

    sort(InputFilenames.begin(), InputFilenames.end());
    sort(AnswerFilenames.begin(), AnswerFilenames.end());
    if (InputFilenames.size() != AnswerFilenames.size())
        throw EXCEPTION("Input and output file size not match");
    configor::json::array_type IOFilenames;
    for (size_t i = 0; i < InputFilenames.size(); i++) {
        std::string InputFilename = InputFilenames[i].substr(0, InputFilenames[i].size() - 3);
        std::string AnswerFilename = AnswerFilenames[i].substr(0, AnswerFilenames[i].size() - 4);
        if (InputFilename != AnswerFilename)
            throw EXCEPTION("Input filename " + InputFilenames[i] + " and output filename " + AnswerFilenames[i] + " not match");
        IOFilenames.push_back({{"InputFilename", InputFilenames[i]},
                               {"AnswerFilename", AnswerFilenames[i]}});
    }
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Upload test case succeeds";
    ResponseJSON["Data"]["IOFilenames"] = IOFilenames;
    return ResponseJSON;
}
configor::json API_PROCEED::UpdateProblem(std::string PID, std::string Title, std::string IOFilename, std::string Description, std::string Input, std::string Output, std::string Range, std::string Hint, std::string Samples, std::string TestGroups) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    PROBLEM Problem;
    Problem.PID = PID;
    Problem.Title = Title;
    Problem.IOFilename = IOFilename;
    Problem.Description = Description;
    Problem.Input = Input;
    Problem.Output = Output;
    Problem.Range = Range;
    Problem.Hint = Hint;
    PROBLEMS::JSONToSamples(Samples, Problem.Samples);
    PROBLEMS::UpdateProblem(Problem);
    RETURN_SUCCESS("Update problem succeeds");
}
configor::json API_PROCEED::UpdateTestCase(std::string PID, std::string TestCase) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    PROBLEMS::UpdateTestCase(PID, TestCase);
    RETURN_SUCCESS("Update test case succeeds");
}
configor::json API_PROCEED::DeleteProblem(std::string PID) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    PROBLEMS::DeleteProblem(PID);
    RETURN_SUCCESS("Delete problem succeeds");
}
configor::json API_PROCEED::GetProblems(int Page) {
    configor::json ResponseJSON = BaseJSON;
    DATABASE::SELECT("Problems")
        .Select("PID")
        .Select("Title")
        .Limit(10)
        .Offset((Page - 1) * 10)
        .Execute(
            [&ResponseJSON](auto Data) {
                ResponseJSON["Success"] = true;
                ResponseJSON["Message"] = "Get problem succeeds";
                configor::json::array_type Problems;
                for (auto i : Data) {
                    configor::json TempProblem;
                    TempProblem["PID"] = i["PID"];
                    TempProblem["Title"] = i["Title"];
                    Problems.push_back(TempProblem);
                }
                ResponseJSON["Data"]["Problems"] = Problems;
            });
    DATABASE::SIZE("Problems")
        .Execute(
            [&ResponseJSON](int Size) {
                ResponseJSON["Data"]["PageCount"] = ceil(Size / 10.0);
            });
    return ResponseJSON;
}

configor::json API_PROCEED::AddSubmission(std::string PID, bool EnableO2, std::string Code) {
    SUBMISSION Submission;
    Submission.Set(Code, PID);
    Submission.EnableO2 = EnableO2;
    Submission.UID = UID;
    SUBMISSIONS::AddSubmission(Submission);
    JudgingList.Add(Submission);
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Add submission succeeds";
    ResponseJSON["Data"]["SID"] = Submission.SID;
    return ResponseJSON;
}
configor::json API_PROCEED::GetSubmission(int SID) {
    SUBMISSION Submission;
    SUBMISSIONS::GetSubmission(SID, Submission);
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Get submission succeeds";
    ResponseJSON["Data"]["EnableO2"] = Submission.EnableO2;
    ResponseJSON["Data"]["Result"] = (int)Submission.Result;
    ResponseJSON["Data"]["Description"] = Submission.Description;
    ResponseJSON["Data"]["PID"] = Submission.PID;
    ResponseJSON["Data"]["UID"] = Submission.UID;
    if (IsAdmin || Submission.UID == UID)
        ResponseJSON["Data"]["Code"] = Submission.Code;
    ResponseJSON["Data"]["Time"] = Submission.Time;
    ResponseJSON["Data"]["TimeSum"] = Submission.TimeSum;
    ResponseJSON["Data"]["Memory"] = Submission.Memory;
    ResponseJSON["Data"]["Score"] = Submission.Score;
    std::string TestGroupsString;
    SUBMISSIONS::TestGroupsToJSON(Submission.TestGroups, TestGroupsString);
    ResponseJSON["Data"]["TestGroups"] = TestGroupsString;
    configor::json::array_type TestGroupsLimits;
    for (auto i : Submission.TestGroups) {
        configor::json TempTestGroup;
        TempTestGroup["TGID"] = i.TGID;
        configor::json::array_type TestCasesLimits;
        for (auto j : i.TestCases) {
            configor::json TempTestCase;
            TempTestCase["TCID"] = j.TCID;
            TempTestCase["TimeLimit"] = j.UnjudgedTestCase->TimeLimit;
            TempTestCase["MemoryLimit"] = j.UnjudgedTestCase->MemoryLimit;
            TestCasesLimits.push_back(TempTestCase);
        }
        TempTestGroup["TestCasesLimits"] = TestCasesLimits;
        TestGroupsLimits.push_back(TempTestGroup);
    }
    ResponseJSON["Data"]["TestGroupsLimits"] = TestGroupsLimits;
    return ResponseJSON;
}
configor::json API_PROCEED::UpdateSubmission(int SID, std::string PID, int UID, std::string Code, int Result, std::string Description, int Time, int TimeSum, int Memory, int Score, bool EnableO2, std::string TestGroups) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    SUBMISSION Submission;
    Submission.SID = SID;
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
    SUBMISSIONS::JSONToTestGroups(TestGroups, Submission.TestGroups, PID, SID);
    SUBMISSIONS::UpdateSubmission(Submission);
    RETURN_SUCCESS("Update problem succeeds");
}
configor::json API_PROCEED::RejudgeSubmission(int SID) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    SUBMISSION Submission;
    SUBMISSIONS::GetSubmission(SID, Submission);
    Submission.Result = JUDGE_RESULT::WAITING;
    Submission.Time = Submission.TimeSum = Submission.Memory = Submission.Score = 0;
    for (auto &i : Submission.TestGroups) {
        i.Result = JUDGE_RESULT::WAITING;
        i.Time = i.TimeSum = i.Memory = i.TestCasesPassed = i.Score = 0;
        for (auto &j : i.TestCases) {
            j.Result = JUDGE_RESULT::WAITING;
            j.Description = "";
            j.Time = j.Memory = j.Score = 0; // TODO: Score here is not correct
        }
    }
    SUBMISSIONS::UpdateSubmission(Submission);
    JudgingList.Add(Submission);
    RETURN_SUCCESS("Rejudge submission succeeds");
}
configor::json API_PROCEED::DeleteSubmission(int SID) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    SUBMISSIONS::DeleteSubmission(SID);
    RETURN_SUCCESS("Delete submission succeeds");
}
configor::json API_PROCEED::GetSubmissions(int Page, int Problem, int User, int Result) {
    configor::json ResponseJSON = BaseJSON;
    DATABASE::SELECT("Submissions")
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
            [&ResponseJSON, Problem, User, Result](auto Data) {
                ResponseJSON["Success"] = true;
                ResponseJSON["Message"] = "Get submissions succeeds";
                configor::json::array_type Submissions;
                for (auto i : Data) {
                    if (Problem != -1 && i["PID"] != std::to_string(Problem))
                        continue;
                    if (User != -1 && i["UID"] != std::to_string(User))
                        continue;
                    if (Result != -1 && i["Result"] != std::to_string(Result))
                        continue;
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
            });
    DATABASE::SIZE("Submissions")
        .Execute(
            [&ResponseJSON](int Size) {
                ResponseJSON["Data"]["PageCount"] = ceil(Size / 10.0);
            });
    return ResponseJSON;
}

configor::json API_PROCEED::GetSettings() {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    configor::json ResponseJSON = BaseJSON;
    ResponseJSON["Success"] = true;
    ResponseJSON["Message"] = "Get settings succeeds";
    SETTINGS::GetSettings(ResponseJSON["Data"]["Settings"]);
    return ResponseJSON;
}
configor::json API_PROCEED::SetSettings(configor::json Settings) {
    if (!IsAdmin)
        throw EXCEPTION("Not admin");
    SETTINGS::SetSettings(Settings);
    RETURN_SUCCESS("Set settings succeeds");
}

configor::json API_PROCEED::Proceed(configor::json Request) {
    configor::json ResponseJSON = BaseJSON;
    try {
        if (!CheckTypes(Request, {{"Action", configor::config_value_type::string}}))
            throw EXCEPTION("Invalid parameters");
        Action = Request["Action"].as_string();
        Data = Request["Data"];
        if (Action == "CheckTokenAvailable") {
            if (!CheckTypes(Data, {{"Token", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = CheckTokenAvailable(Data["Token"].as_string());
        } else if (Action == "Register") {
            if (!CheckTypes(Data, {{"Username", configor::config_value_type::string},
                                   {"Nickname", configor::config_value_type::string},
                                   {"Password", configor::config_value_type::string},
                                   {"EmailAddress", configor::config_value_type::string},
                                   {"VerificationCode", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = Register(Data["Username"].as_string(),
                                        Data["Nickname"].as_string(),
                                        Data["Password"].as_string(),
                                        Data["EmailAddress"].as_string(),
                                        Data["VerificationCode"].as_string());
        } else if (Action == "CheckUsernameAvailable") {
            if (!CheckTypes(Data, {{"Username", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = CheckUsernameAvailable(Data["Username"].as_string());
        } else if (Action == "CheckEmailAvailable") {
            if (!CheckTypes(Data, {{"EmailAddress", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = CheckEmailAvailable(Data["EmailAddress"].as_string());
        } else if (Action == "SendVerificationCode") {
            if (!CheckTypes(Data, {{"EmailAddress", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = SendVerificationCode(Data["EmailAddress"].as_string());
        } else if (Action == "Login") {
            if (!CheckTypes(Data, {{"Username", configor::config_value_type::string},
                                   {"Password", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = Login(Data["Username"].as_string(),
                                     Data["Password"].as_string());
        } else if (Action == "ResetPassword") {
            if (!CheckTypes(Data, {{"EmailAddress", configor::config_value_type::string},
                                   {"VerificationCode", configor::config_value_type::string},
                                   {"Password", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = ResetPassword(Data["EmailAddress"].as_string(),
                                             Data["VerificationCode"].as_string(),
                                             Data["Password"].as_string());
        } else if (Action == "CreatePasskeyChallenge") {
            if (!CheckTypes(Data, {}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = CreatePasskeyChallenge();
        } else if (Action == "DeletePasskeyChallenge") {
            if (!CheckTypes(Data, {{"Challenge", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = DeletePasskeyChallenge(Data["Challenge"].as_string());
        } else if (Action == "LoginWithPasskey") {
            if (!CheckTypes(Data, {{"Challenge", configor::config_value_type::string},
                                   {"CredentialID", configor::config_value_type::string},
                                   {"UID", configor::config_value_type::number_integer},
                                   {"CredentialSignature", configor::config_value_type::string}}))
                ResponseJSON["Message"] = "Invalid parameters";
            else
                ResponseJSON = LoginWithPasskey(Data["Challenge"].as_string(),
                                                Data["CredentialID"].as_string(),
                                                Data["UID"].as_integer(),
                                                Data["CredentialSignature"].as_string());
        } else {
            if (!CheckTypes(Data, {{"Token", configor::config_value_type::string}}))
                throw EXCEPTION("Invalid parameters");
            Token = Data["Token"].as_string();
            if (!CheckTokenAvailable(Token)["Success"].as_bool())
                throw EXCEPTION("Invalid token");

            UID = TOKENS::GetUID(Token);
            IsAdmin = USERS::IsAdmin(UID);

            if (Action == "CreatePasskey") {
                if (!CheckTypes(Data, {{"Challenge", configor::config_value_type::string},
                                       {"CredentialID", configor::config_value_type::string},
                                       {"CredentialPublicKey", configor::config_value_type::string}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = CreatePasskey(Data["Challenge"].as_string(),
                                                 Data["CredentialID"].as_string(),
                                                 Data["CredentialPublicKey"].as_string());
            } else if (Action == "AddUser") {
                if (!CheckTypes(Data, {{"Username", configor::config_value_type::string},
                                       {"Nickname", configor::config_value_type::string},
                                       {"Password", configor::config_value_type::string},
                                       {"EmailAddress", configor::config_value_type::string},
                                       {"Role", configor::config_value_type::number_integer}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = AddUser(Data["Username"].as_string(),
                                           Data["Nickname"].as_string(),
                                           Data["Password"].as_string(),
                                           Data["EmailAddress"].as_string(),
                                           (USER_ROLE)Data["Role"].as_integer());
            } else if (Action == "GetUser") {
                if (!CheckTypes(Data, {{"UID", configor::config_value_type::number_integer}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = GetUser(Data["UID"].as_integer());
            } else if (Action == "UpdateUser") {
                if (!CheckTypes(Data, {{"UID", configor::config_value_type::number_integer},
                                       {"Username", configor::config_value_type::string},
                                       {"Nickname", configor::config_value_type::string},
                                       {"Password", configor::config_value_type::string},
                                       {"EmailAddress", configor::config_value_type::string},
                                       {"Role", configor::config_value_type::number_integer}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = UpdateUser(Data["UID"].as_integer(),
                                              Data["Username"].as_string(),
                                              Data["Nickname"].as_string(),
                                              Data["Password"].as_string(),
                                              Data["EmailAddress"].as_string(),
                                              (USER_ROLE)Data["Role"].as_integer());
            } else if (Action == "DeleteUser") {
                if (!CheckTypes(Data, {{"UID", configor::config_value_type::number_integer}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = DeleteUser(Data["UID"].as_integer());
            } else if (Action == "GetUsers") {
                if (!CheckTypes(Data, {{"Page", configor::config_value_type::number_integer}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = GetUsers(Data["Page"].as_integer());
            } else if (Action == "HashPassword") {
                if (!CheckTypes(Data, {{"OriginalPassword", configor::config_value_type::string}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = HashPassword(Data["OriginalPassword"].as_string());
            } else if (Action == "AddProblem") {
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
            } else if (Action == "UploadTestCase") {
                if (!CheckTypes(Data, {{"PID", configor::config_value_type::string},
                                       {"Data", configor::config_value_type::string}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = UploadTestCase(Data["PID"].as_string(),
                                                  Data["Data"].as_string());
            } else if (Action == "UpdateTestCase") {
                if (!CheckTypes(Data, {{"PID", configor::config_value_type::string},
                                       {"TestCase", configor::config_value_type::string}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = UpdateTestCase(Data["PID"].as_string(),
                                                  Data["TestCase"].as_string());
            } else if (Action == "GetProblem") {
                if (!CheckTypes(Data, {{"PID", configor::config_value_type::string}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = GetProblem(Data["PID"].as_string());
            } else if (Action == "UpdateProblem") {
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
            } else if (Action == "DeleteProblem") {
                if (!CheckTypes(Data, {{"PID", configor::config_value_type::string}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = DeleteProblem(Data["PID"].as_string());
            } else if (Action == "GetProblems") {
                if (!CheckTypes(Data, {{"Page", configor::config_value_type::number_integer}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = GetProblems(Data["Page"].as_integer());
            } else if (Action == "AddSubmission") {
                if (!CheckTypes(Data, {{"PID", configor::config_value_type::string},
                                       {"EnableO2", configor::config_value_type::boolean},
                                       {"Code", configor::config_value_type::string}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = AddSubmission(Data["PID"].as_string(),
                                                 Data["EnableO2"].as_bool(),
                                                 Data["Code"].as_string());
            } else if (Action == "GetSubmission") {
                if (!CheckTypes(Data, {{"SID", configor::config_value_type::number_integer}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = GetSubmission(Data["SID"].as_integer());
            } else if (Action == "UpdateSubmission") {
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
            } else if (Action == "RejudgeSubmission") {
                if (!CheckTypes(Data, {{"SID", configor::config_value_type::number_integer}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = RejudgeSubmission(Data["SID"].as_integer());
            } else if (Action == "DeleteSubmission") {
                if (!CheckTypes(Data, {{"SID", configor::config_value_type::number_integer}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = DeleteSubmission(Data["SID"].as_integer());
            } else if (Action == "GetSubmissions") {
                if (!CheckTypes(Data, {{"Page", configor::config_value_type::number_integer},
                                       {"Problem", configor::config_value_type::number_integer},
                                       {"User", configor::config_value_type::number_integer},
                                       {"Result", configor::config_value_type::number_integer}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = GetSubmissions(Data["Page"].as_integer(), Data["Problem"].as_integer(), Data["User"].as_integer(), Data["Result"].as_integer());
            } else if (Action == "GetSettings") {
                ResponseJSON = GetSettings();
            } else if (Action == "SetSettings") {
                if (!CheckTypes(Data, {{"Settings", configor::config_value_type::object}}))
                    ResponseJSON["Message"] = "Invalid parameters";
                else
                    ResponseJSON = SetSettings(Data["Settings"]);
            } else
                ResponseJSON["Message"] = "No such action";
        }
    } catch (EXCEPTION ErrorData) {
        ResponseJSON["Success"] = false;
        ResponseJSON["Message"] = ErrorData.Message;
        ResponseJSON["Data"] = configor::json();
    }
    return ResponseJSON;
}
