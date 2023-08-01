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
    srand(time(NULL));
    OUTPUT_IF_FAILED(Settings.Init());
    JudgingList.Init();
    new SOCKET(
        [](std::string RequestHTTPData) -> std::string
        {
            WEB_DATA_PROCEED Data;
            return Data.Proceed(HTTP_REQUEST(RequestHTTPData)).Stringify();
        });
    return 0;
}
