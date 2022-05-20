#include "Uart.hpp"
#include "esp_log.h"
#include "MutexLocker.hpp"
#include "soc/uart_reg.h"
#include <cstring>
#include "AsyncFunctor.hpp"
#include "TimeUtils.hpp"

#define UART_EVENT_QUEUE_LENGTH 20
#define UART_IDLE_PERIOD_MS     50
#define UART_READ_TIMEOUT_MS    100

static const char* const TAG {"uart"};

Uart::Uart(const int uartNumber, const uart_config_t cfg, const size_t bufferSize)
    :   _eventQueue{},
        _uartNumber{uartNumber},
        _mutex{xSemaphoreCreateRecursiveMutex()} {
    
    assert(_mutex != NULL);
    ESP_ERROR_CHECK(uart_driver_install(_uartNumber, bufferSize * 2, bufferSize * 2, UART_EVENT_QUEUE_LENGTH, &_eventQueue, 0));
    assert(_eventQueue != NULL);
    ESP_ERROR_CHECK(uart_param_config(_uartNumber, &cfg));
    ESP_ERROR_CHECK(remapPins(UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    assert(pdTRUE == xTaskCreate(uartEventTask, "uart_event_task", configMINIMAL_STACK_SIZE*4, this, 12, NULL)); 
}

bool Uart::readByte(char& byte) {
    return uart_read_bytes(_uartNumber, &byte, 1, pdMS_TO_TICKS(UART_READ_TIMEOUT_MS));
}

bool Uart::readByte(uint8_t& byte) {
    return uart_read_bytes(_uartNumber, &byte, 1, pdMS_TO_TICKS(UART_READ_TIMEOUT_MS));
}

std::string Uart::readAll() {
    const size_t available {bytesAvailable()};
    //ESP_LOGI(TAG, "bytes available: %u", available);
    std::string ret (available, '\0');
    uart_read_bytes(_uartNumber, const_cast<char*>(ret.data()), available, pdMS_TO_TICKS(UART_READ_TIMEOUT_MS));  
    return ret;
}

esp_err_t Uart::remapPins(const int tx, const int rx, const int rts, const int cts) {
    MutexLocker locker{_mutex};
    return uart_set_pin(_uartNumber, tx, rx, rts, cts);
}

void Uart::uartEventTask(void* args) {
    
    Uart* uart = static_cast<Uart*>(args);
    uart_event_t event{};

    auto idleDetectionTask = [&uart]() {
        while(uart->_lastTimeDataReceived and false == TimeUtils::isPeriodPassed(uart->_lastTimeDataReceived, UART_IDLE_PERIOD_MS)) {
            vTaskDelay(UART_IDLE_PERIOD_MS/portTICK_RATE_MS);
        }
        // printf("idle detected!\n");
        if (uart->_onDataReceivedCb) {
            uart->_onDataReceivedCb();
        }
    };
    AsyncFunctor idleDetector{idleDetectionTask, "idleDetectionTask", configMINIMAL_STACK_SIZE*3};

    while(1) {
      if(xQueueReceive(uart->_eventQueue, (void*)&event, (portTickType)portMAX_DELAY)) {
        MutexLocker locker{uart->_mutex};
        // printf("uart[%d] event: %u\n", uart->_uartNumber, event.type);
        switch(event.type) {
            
            case UART_DATA:
                //ESP_LOGI(TAG, "data received, %u bytes", event.size);
                uart->_lastTimeDataReceived = TimeUtils::nowMs();
                if (!idleDetector.isRunning()) {
                    idleDetector.start();
                }
            break;

            case UART_FIFO_OVF:
                ESP_LOGE(TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(uart->_uartNumber);
                xQueueReset(uart->_eventQueue);
            break;
            //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                ESP_LOGE(TAG, "ring buffer full");
                // If buffer full happened, you should consider encreasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(uart->_uartNumber);
                xQueueReset(uart->_eventQueue);
            break;

         //Event of UART RX break detected
        // case UART_BREAK:
        //     ESP_LOGI(TAG, "uart rx break");
        //     break;
        // //Event of UART parity check error
        // case UART_PARITY_ERR:
        //     ESP_LOGI(TAG, "uart parity error");
        //     break;
        // //Event of UART frame error
        // case UART_FRAME_ERR:
        //     ESP_LOGI(TAG, "uart frame error");
        //     break;
        // //UART_PATTERN_DET
        // // case UART_PATTERN_DET:{
        // //     uart_get_buffered_data_len(/*uart->_uartNumber*/0, &buffered_size);
        // //     int pos = uart_pattern_pop_pos(/*uart->_uartNumber*/0);
        // //     ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
        // //     if (pos == -1) {
        // //         // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
        // //         // record the position. We should set a larger queue size.
        // //         // As an example, we directly flush the rx buffer here.
        // //         uart_flush_input(/*uart->_uartNumber*/0);
        // //     } else {
        // //         uart_read_bytes(/*uart->_uartNumber*/0, dtmp, pos, 100 / portTICK_PERIOD_MS);
        // //         uint8_t pat[PATTERN_CHR_NUM + 1];
        // //         memset(pat, 0, sizeof(pat));
        // //         uart_read_bytes(/*uart->_uartNumber*/0, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
        // //         ESP_LOGI(TAG, "read data: %s", dtmp);
        // //         ESP_LOGI(TAG, "read pat : %s", pat);
        // //     }
        // //     break;
        // // }
        // //Others
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
            break;
        }
      }
    }
    vTaskDelete(NULL);
}

size_t Uart::bytesAvailable() const {
    MutexLocker locker{_mutex};
    size_t available{};
    uart_get_buffered_data_len(_uartNumber, &available);
    return available;
}

void Uart::read(uint8_t* out, const size_t length) {
    uart_read_bytes(_uartNumber, out, length, pdMS_TO_TICKS(UART_READ_TIMEOUT_MS));
}

size_t Uart::write(const uint8_t* data, const size_t length) {
    return uart_write_bytes(_uartNumber, data, length);
}

size_t Uart::write(const char* str) {
    return uart_write_bytes(_uartNumber, str, strlen(str));
}

size_t Uart::write(const std::string& str) {
    return uart_write_bytes(_uartNumber, str.c_str(), str.length());
}

void Uart::flush() {
    uart_flush(_uartNumber);
}