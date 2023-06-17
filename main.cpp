#include "WebDataProceed.hpp"
#include "Socket.hpp"
#include "Problems.hpp"
#include "Submission.hpp"
#include "JudgingList.hpp"
#include "unistd.h"
#include "HTTPRequest.hpp"
#include "Utilities.hpp"
#include "Database.hpp"

int main()
{
    // Settings.CheckSettings();
    // PROBLEM Problem;
    // Problem.Load("1000");
    // PROBLEMS::Load();
    // AddProblem();
    // Settings.Save();
    // return 0;

    srand(time(NULL));
    OUTPUT_IF_FAILED(Settings.Load("Judger"));
    Settings.CheckSettings();
    JudgingList.Init();

    new SOCKET(
        [](std::string RequestHTTPData) -> std::string
        {
            WEB_DATA_PROCEED Data;
            return Data.Proceed(HTTP_REQUEST(RequestHTTPData)).Stringify();
        });
    return 0;
}
