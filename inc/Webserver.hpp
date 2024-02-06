#pragma once

#include <esp_http_server.h>
#include <string>
#include <functional>
#include <vector>
#include <map>


using uriHandler = esp_err_t(*)(httpd_req_t*);
using errorHandler = esp_err_t (*)(httpd_req_t *req, httpd_err_code_t error);
struct Webserver {
    explicit Webserver(const httpd_config&& cfg = HTTPD_DEFAULT_CONFIG());
    esp_err_t start();
    esp_err_t stop();
    esp_err_t setUriHandler(const char* const uri, const httpd_method_t method, uriHandler handler);
    esp_err_t setErrorHandler(const httpd_err_code_t code, errorHandler handler);
    static std::string getUriQuery(httpd_req* const req);
    static std::string getContent(httpd_req* const req);
    static std::string getUrl(httpd_req* const req);
    static std::string getHeader(httpd_req* const req, const char* header);
    
private:
    httpd_handle_t _server;
    httpd_config _config;
    std::vector<httpd_uri_t> _handlers;
    std::map<httpd_err_code_t, errorHandler> _errHandlers;
};