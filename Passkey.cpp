#include "Passkey.hpp"
#include "Database.hpp"
#include "Utilities.hpp"

std::string PASSKEY::CreateChallenge()
{
    std::string Challenge = UTILITIES::RandomToken();
    DATABASE::INSERT("PasskeyChallenges")
        .Insert("Challenge", Challenge)
        .Execute();
    return Challenge;
}
void PASSKEY::DeleteChallenge(std::string Challenge)
{
    DATABASE::DELETE("PasskeyChallenges")
        .Where("Challenge", Challenge)
        .Execute();
}
void PASSKEY::CreatePasskey(int UID, std::string Challenge, std::string Credential, std::string PublicKey)
{
    DATABASE::SELECT("PasskeyChallenges")
        .Select("CreateTime")
        .Where("Challenge", Challenge)
        .Execute(
            [Challenge](auto Data)
            {
                if (Data.size() != 1)
                    throw EXCEPTION("Invalid challenge");
                DATABASE::DELETE("PasskeyChallenges")
                    .Where("Challenge", Challenge)
                    .Execute();
                if (UTILITIES::StringToTime(Data[0]["CreateTime"]) + 60 < time(NULL))
                    throw EXCEPTION("Challenge expired");
            });
    DATABASE::INSERT("Passkeys")
        .Insert("UID", UID)
        .Insert("Credential", Credential)
        .Insert("PublicKey", PublicKey)
        .Execute();
}
std::string PASSKEY::GetPasskey(int UID, std::string Credential)
{
    std::string Passkey;
    DATABASE::SELECT("Passkeys")
        .Select("PublicKey")
        .Where("UID", UID)
        .Where("Credential", Credential)
        .Execute(
            [&Passkey](auto Data)
            {
                if (Data.size() != 1)
                    throw EXCEPTION("Invalid credential");
                Passkey = Data[0]["PublicKey"];
            });
    return Passkey;
}
