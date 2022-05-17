#pragma once

#include "freertos/FreeRTOS.h"

class InterruptLock {
    public:
        InterruptLock() {
            _timeCriticalMutex = portMUX_INITIALIZER_UNLOCKED;
            portENTER_CRITICAL(&_timeCriticalMutex);
        }    
        ~InterruptLock() {
            portEXIT_CRITICAL(&_timeCriticalMutex);
        }
    private:
        portMUX_TYPE _timeCriticalMutex;
};