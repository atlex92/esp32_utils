#include "Webserver.hpp"
#include "esp_log.h"

static const char* const TAG{"WEB_SERVER"};

Webserver::Webserver(const httpd_config&& cfg) : _server{}, _config(std::move(cfg)) {

}

esp_err_t Webserver::start() {
    esp_err_t ret {ESP_OK};

    ret = httpd_start(&_server, &_config);
    
    if(ESP_OK != ret) {
        ESP_LOGE(TAG, "httpd_start failed with code: %s", esp_err_to_name(ret));
        return ret;
    }
    
    for(const auto &handler : _handlers) {
        ret = httpd_register_uri_handler(_server, &handler);
        if(ESP_OK != ret) {
            ESP_LOGE(TAG, "failed to register handler for uri: %s, error:%s", handler.uri, esp_err_to_name(ret));
            return ret;
        }
    }
    return ret;
}

esp_err_t  Webserver::stop() {
    return httpd_stop(_server);
}

esp_err_t Webserver::setUriHandler(const char* const uri, const httpd_method_t method, uriHandler handler) {
    _handlers.emplace_back(httpd_uri_t{uri, method, handler, this});
    return httpd_register_uri_handler(_server, &_handlers.back());
}

esp_err_t Webserver::setErrorHandler(const httpd_err_code_t code, errorHandler handler) {
    _errHandlers[code] = handler;
    return httpd_register_err_handler(_server, code, handler);
}

std::string Webserver::getUriQuery(httpd_req* const req) {
    if(!req)
        return "";

    const size_t queryLength{httpd_req_get_url_query_len(req)};
    std::string ret;
    ret.resize(queryLength + 1);
    httpd_req_get_url_query_str(req, const_cast<char*>(ret.data()), queryLength + 1);
    return ret;
}

std::string Webserver::getUrl(httpd_req* const req) {
    if(!req)
        return "";
    return req->uri;
}

std::string Webserver::getContent(httpd_req* const req) {
    if(!req)
        return "";
    
    const size_t contentLen{req->content_len};
    ESP_LOGI(TAG, "contentLen: %u", contentLen);

    std::string ret;
    ret.resize(contentLen);

    size_t remaining {contentLen};
    int bytesRead{};
    while (remaining > 0) {
        if ((bytesRead = httpd_req_recv(req, const_cast<char*>(ret.data()), remaining)) <= 0) {
            if (bytesRead == HTTPD_SOCK_ERR_TIMEOUT) {
                ESP_LOGE(TAG, "HTTPD_SOCK_ERR_TIMEOUT");
                continue;
            }
            ESP_LOGE(TAG, "httpd_req_recv error: %d", bytesRead);
            return "";
        }
        ESP_LOGI(TAG, "bytesRead: %d, remaining: %u", bytesRead, remaining);
        remaining -= bytesRead;
    }

    return ret;
}

std::string Webserver::getHeader(httpd_req* const req, const char* header) {
    const size_t headerLength{httpd_req_get_hdr_value_len(req, header)};
    std::string ret;
    ret.resize(headerLength);
    httpd_req_get_hdr_value_str(req, header, (char*)ret.data(), headerLength + 1);
    return ret;
}
