#pragma once

#include <string>
#include <vector>
#include <functional>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "mutex.hpp"
#include "driver/uart.h"
#include "driver/gpio.h"


#define UART_DEFAULT_BUFFER_SIZE    (1024)

#define UART_CFG_DEFAULT(baud)    ( uart_config_t {\
    .baud_rate = baud,\
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
        QueueHandle_t _eventQueue;
        const int _uartNumber;
        std::function<void()> _onDataReceivedCb {nullptr};
        mutable Mutex _mutex;
        uint32_t _lastTimeDataReceived{};
};