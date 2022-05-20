#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <vector>
#include <functional>

#define UART_DEFAULT_BUFFER_SIZE    (1024)

#define UART_CFG_DEFAULT    ( uart_config_t {\
    .baud_rate = 115200,\
    .data_bits = UART_DATA_8_BITS,\
    .parity = UART_PARITY_DISABLE,\
    .stop_bits = UART_STOP_BITS_1,\
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,\
    .source_clk = UART_SCLK_APB,\
  })

struct Uart {

    friend void IRAM_ATTR uart_intr_handle(void *arg);
    explicit Uart(const int uartNumber, const uart_config_t cfg, const size_t bufferSize = UART_DEFAULT_BUFFER_SIZE);
    esp_err_t remapPins(const int tx, const int rx, const int rts, const int cts);
    size_t bytesAvailable() const;
    void onDataReceived(std::function<void()> cb) {
      _onDataReceivedCb = cb;
    }
    bool readByte(char& byte);
    bool readByte(uint8_t& byte);
    std::string readAll();
    void read(uint8_t* out, const size_t length);

    size_t write(const uint8_t* data, const size_t length);
    size_t write(const char* str);
    size_t write(const std::string& str);

    void flush();

    private:
        static void uartEventTask(void* args);
        void mutexLock();
        void mutexUnlock();
        xQueueHandle _eventQueue;
        const int _uartNumber;
        std::function<void()> _onDataReceivedCb {nullptr};
        SemaphoreHandle_t _mutex;
        uint32_t _lastTimeDataReceived{};
};