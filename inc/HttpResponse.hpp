#pragma once

#include <string>
#include <map>
#include "HttpCodes.hpp"

using stringMap = std::map<std::string, std::string>;
struct HttpResponse {
    std::string body;
    stringMap headers;
    eHttpCode code;
};