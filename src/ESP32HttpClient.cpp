#include "ESP32HttpClient.hpp"
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"

#define CONNECTION_HEADER                   "Connection"
#define CONTENT_TYPE_HEADER                 "Content-Type"
#define DEFAULT_BUFFER_SIZE                 2048UL
#define MAX_RESPONSE_BODY_LENGTH            2048UL
#define MAX_REDIRECTIONS                    3
#define MAX_AUTH_TRIES                      3

static const char* const TAG = "ESP32HttpClient";

esp_err_t ESP32HttpClient::httpClientEventHandler(esp_http_client_event_t *evt) {

    HttpRequest* req = static_cast<HttpRequest*>(evt->user_data);
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
        break;

        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;

        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;

        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER: key = %s, value = %s", evt->header_key, evt->header_value);
            req->setHeader(evt->header_key, evt->header_value);
        break;
        
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            req->response()->body.append((char*)evt->data, evt->data_len);
        break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;

        case HTTP_EVENT_DISCONNECTED:
        {
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
        }
        break;

        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

ESP32HttpClient::ESP32HttpClient() : ESP32HttpClient{DEFAULT_BUFFER_SIZE, MAX_RESPONSE_BODY_LENGTH} {

}

ESP32HttpClient::ESP32HttpClient(const size_t bufferSize, const size_t maxResponseBodyLength)
    :   _client{}, _bufferSize{bufferSize}, _maxResponseBodyLength{maxResponseBodyLength} {

}

bool ESP32HttpClient::GET (HttpRequest& req) {
    return performRequest(req, esp_http_client_method_t::HTTP_METHOD_GET);
}

bool ESP32HttpClient::POST (HttpRequest& req) {
    return performRequest(req, esp_http_client_method_t::HTTP_METHOD_POST);
}

void ESP32HttpClient::setCertificate(const char* const cert, const size_t length) {
    _cert = cert;
    _certLength = length;
}

bool ESP32HttpClient::initClient(HttpRequest& req) {

    resetClient();

    const std::string username{req.username()};
    const std::string password{req.password()};

    esp_http_client_config_t config = {
        .url = req.url().c_str(),
        .port = (int)(req.port()),
        .username = username.length() ? username.c_str() : nullptr,
        .password = password.length() ? password.c_str() : nullptr,
        .auth_type = req.authType(),
        .cert_pem = _cert,
        .cert_len = _certLength,
        .timeout_ms = (int)(req.timeoutMs()),
        .max_redirection_count = MAX_REDIRECTIONS,
        .max_authorization_retries = MAX_AUTH_TRIES,
        .event_handler = httpClientEventHandler,
        .buffer_size = (int)_bufferSize,
        .buffer_size_tx = (int)_bufferSize,
        .user_data = &req,
        .is_async = false,
        .crt_bundle_attach = esp_crt_bundle_attach
    };

    _client = esp_http_client_init(&config);

    if (!_client)
        return false;

    if (true == req.keepConnection()) {
        req.setHeader(CONNECTION_HEADER, "keep-alive");
    }

    for (auto &header : req.headers()) {
        esp_http_client_set_header(_client, header.first.c_str(), header.second.c_str());
    }

    return true;
}

const char* ESP32HttpClient::reqStatusString(const eRequestStatus val) const{
    switch (val) {
        case eRequestStatus::REQUEST_STATUS_FAILED:
            return "FAILED";
        case eRequestStatus::REQUEST_STATUS_OK:
            return "OK";
        case eRequestStatus::REQUEST_STATUS_IDDLE:
            return "IDDLE";
        case eRequestStatus::REQUEST_STATUS_PENDING:
            return "PENDING";
        default:
            return "UNKNOWN";
        break;
    }
}

bool ESP32HttpClient::performRequest(HttpRequest& req, const esp_http_client_method_t method) {
    if(!initClient(req)) {
        req.set_status(eRequestStatus::REQUEST_STATUS_FAILED);
        return false;
    }

    req.set_status(eRequestStatus::REQUEST_STATUS_PENDING);

    ESP_ERROR_CHECK(esp_http_client_set_method(_client, method));

    if(req.body().length()) {
        ESP_ERROR_CHECK(esp_http_client_set_post_field(_client, req.body().c_str(), req.body().length()));
    }

    if (!req.response()) {
        req.createResponse();
    }
    else {
        ESP_LOGE(TAG, "repsponse already exists!");
    }
    esp_err_t err = esp_http_client_perform(_client);

    req.response()->code = (eHttpCode) esp_http_client_get_status_code(_client);
    if (err == ESP_OK) {
        const auto contentLength {esp_http_client_get_content_length(_client)};
        ESP_LOGI(TAG, "httpCode: %d, content length: %lld", req.response()->code, contentLength);
        req.set_status(eRequestStatus::REQUEST_STATUS_OK);
    }
    else {
        req.set_status(eRequestStatus::REQUEST_STATUS_FAILED);
        ESP_LOGE(TAG, "request failed!");
        return false;
    }
    return true;
}

ESP32HttpClient::~ESP32HttpClient() {
    resetClient();
    ESP_LOGI(TAG, "~ESP32HttpClient()");
}

void ESP32HttpClient::resetClient() {
    esp_http_client_cleanup(_client);
    _client = nullptr;
}