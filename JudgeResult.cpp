#include "JudgeResult.hpp"
#include <string>

std::string GetJudgeResultString(JUDGE_RESULT JudgeResult)
{
    switch (JudgeResult)
    {
    case UNKNOWN_ERROR:
        return "Unknown Error";
    case ACCEPTED:
        return "Accepted";
    case PRESENTATION_ERROR:
        return "Presentation Error";
    case WRONG_ANSWER:
        return "Wrong Answer";
    case TIME_LIMIT_EXCEEDED:
        return "Time Limit Exceeded";
    case MEMORY_LIMIT_EXCEEDED:
        return "Memory Limit Exceeded";
    case OUTPUT_LIMIT_EXCEEDED:
        return "Output Limit Exceeded";
    case RUNTIME_ERROR:
        return "Runtime Error";
    case RESTRICTED_FUNCTION:
        return "Restricted Function";
    case COMPILATION_ERROR:
        return "Compilation Error";
    case SYSTEM_ERROR:
        return "System Error";
    case WAITING:
        return "Waiting";
    case FETCHED:
        return "Fetched";
    case COMPILING:
        return "Compiling";
    case COMPILED:
        return "Compiled";
    case JUDGING:
        return "Judging";
    case JUDGED:
        return "Judged";
    case COMPARING:
        return "Comparing";
    case SKIPPED:
        return "Skipped";
    case REJECTED:
        return "Rejected";
    }
    return "";
}
std::string GetJudgeResultColorString(JUDGE_RESULT JudgeResult)
{
    std::string ColorString = "\033[1;";
    switch (JudgeResult)
    {
    case PRESENTATION_ERROR:
    case WRONG_ANSWER:
        ColorString += "31";
        break;
    case ACCEPTED:
        ColorString += "32";
        break;
    case TIME_LIMIT_EXCEEDED:
    case COMPILATION_ERROR:
        ColorString += "33";
        break;
    case MEMORY_LIMIT_EXCEEDED:
    case SYSTEM_ERROR:
        ColorString += "34";
        break;
    case RESTRICTED_FUNCTION:
    case UNKNOWN_ERROR:
        ColorString += "35";
        break;
    case OUTPUT_LIMIT_EXCEEDED:
    case RUNTIME_ERROR:
        ColorString += "36";
        break;
    default:
        ColorString += "37";
        break;
    }
    return ColorString + "m" + GetJudgeResultString(JudgeResult) + "\033[0m";
}
