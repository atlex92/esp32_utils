#pragma once

#include <stdint.h>
#include "esp_system.h"

namespace ESP32Utils {
    void reboot();
    uint32_t chipID();
    esp_reset_reason_t rebootReason();
    uint32_t millis();
};