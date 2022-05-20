#pragma once

#include <string>
#include <map>
#include <stdint.h>
#include <memory>
#include "HttpRequest.hpp"

struct ESP32HttpClient {

    public:
        virtual ~ESP32HttpClient();
        explicit ESP32HttpClient();
        explicit ESP32HttpClient(const size_t bufferSize, const size_t maxResponseBodyLength);
        bool GET (HttpRequest& req);
        bool POST (HttpRequest& req);
        void setCertificate(const char* const cert, const size_t length);
    private:
        bool performRequest(HttpRequest& req, const esp_http_client_method_t method);
        void resetClient();
        bool initClient(HttpRequest& reqInfo);
        static esp_err_t httpClientEventHandler(esp_http_client_event_t *evt);
        const char* reqStatusString(const eRequestStatus val) const;
        esp_http_client_handle_t _client;
        size_t _bufferSize;
        size_t _maxResponseBodyLength;
        const char* _cert{nullptr};
        size_t _certLength{};
};