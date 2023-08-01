#pragma once

#include <string>
#include "HTTPRequest.hpp"
#include "configor/json.hpp"
#include "HTTPResponse.hpp"

class WEB_DATA_PROCEED
{
public:
    HTTP_RESPONSE Proceed(HTTP_REQUEST HTTPRequest);
};
