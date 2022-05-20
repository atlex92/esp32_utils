#include "HttpRequest.hpp"
#include "HttpUtils.hpp"
#include "StringUtils.hpp"

#define HTTP_DEFAULT_TIMEOUT_MS     30000UL
#define HTTP_DEFAULT_PORT           80UL

static const char* const TAG {"HttpRequest"};

HttpRequest::HttpRequest (const std::string& uri, const std::string& body) 
    :   _url{uri},
        _body{}, 
        _timeoutMs{HTTP_DEFAULT_TIMEOUT_MS},
        _keepConnection{true},
        _port{HTTP_DEFAULT_PORT},
        _username{}, _password{}, _status{},
        _authType{} {
    set_body(body);
}

HttpRequest::HttpRequest (const std::string& uri) 
    :   HttpRequest(uri, {}) {

}

HttpRequest::~HttpRequest() {
    ESP_LOGI(TAG, "~HttpRequest, url: %s", _url.c_str());
}

bool HttpRequest::isValid() const {

    std::string protocol;

    if (false == HttpUtils::getProtocolFromUrl(_url, protocol))
        return false;
    
    if (false == isProtocolValid(protocol))
        return false;
            
    return true;
}

void HttpRequest::setHeader(const std::string& header, const std::string& value) {
    _headers[header] = value;
}

void HttpRequest::addParamInQuery(const std::string  key, std::string value) {
    _params[key] = value;
}

bool HttpRequest::isProtocolValid(const std::string& protocol) const {
    return StringUtils::caseInsEquals(protocol,"http") or StringUtils::caseInsEquals(protocol,"https");
}

void HttpRequest::setContentType(const char* const value) {
    setHeader("Content-Type", value);
}