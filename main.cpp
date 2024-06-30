/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2024  langningchen

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

#include "Database.hpp"
#include "HTTPRequest.hpp"
#include "JudgingList.hpp"
#include "Problems.hpp"
#include "Socket.hpp"
#include "Submission.hpp"
#include "Utilities.hpp"
#include "WebDataProceed.hpp"
#include <unistd.h>

int main() {
    srand(time(NULL));
    Settings.Init();
    JudgingList.Init();
    new SOCKET(
        [](std::string RequestHTTPData) -> std::string {
            WEB_DATA_PROCEED Data;
            HTTP_REQUEST HTTPRequest;
            HTTP_RESPONSE HTTPResponse;
            try {
                HTTPRequest.Parse(RequestHTTPData);
                HTTPResponse = Data.Proceed(HTTPRequest);
            } catch (EXCEPTION ErrorData) {
                HTTPResponse.SetCode(400);
            } catch (std::exception &ErrorData) {
                HTTPResponse.SetCode(400);
                Logger.Error(ErrorData.what());
            }
            return HTTPResponse.Stringify();
        });
    return 0;
}
