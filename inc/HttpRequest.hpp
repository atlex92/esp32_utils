#pragma once

#include <vector>
#include <memory>
#include "HttpResponse.hpp"
#include "Properties.hpp"
#include "esp_log.h"
#include "esp_http_client.h"

using sharedResponse = std::shared_ptr<HttpResponse>;

typedef enum {
    REQUEST_STATUS_IDDLE,
    REQUEST_STATUS_OK,
    REQUEST_STATUS_FAILED,
    REQUEST_STATUS_PENDING,
} eRequestStatus;

struct HttpRequest {
    HttpRequest (const std::string& url);
    HttpRequest (const std::string& url, const std::string& body);
    ~HttpRequest();
    bool isValid() const;
    void setHeader(const std::string& header, const std::string& value);
    const stringMap& headers() const {
        return _headers;
    }
    const std::string& url()const { return _url; }
    sharedResponse response() const { return _response; }
    sharedResponse createResponse() {return _response = std::make_shared<HttpResponse>();}
    void addParamInQuery(const std::string  key, std::string value);
    void set_body(const std::string& body) {_body = body; }
    const std::string& body() const {return _body;}
    void setContentType(const char* const value);
private:
    bool isProtocolValid(const std::string& protocol) const;
    stringMap _params;
    std::string _url;
    std::string _body;
    PROPERTY(uint32_t, timeoutMs)
    PROPERTY(bool, keepConnection)
    PROPERTY(size_t, port)
    PROPERTY(std::string, username)
    PROPERTY(std::string, password)
    PROPERTY(eRequestStatus, status)
    PROPERTY(esp_http_client_auth_type_t, authType)
    stringMap _headers{};
    sharedResponse _response{};
};