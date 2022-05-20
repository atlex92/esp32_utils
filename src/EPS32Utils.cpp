
#include "ESP32Utils.hpp"
#include "esp_system.h"
#include "esp_timer.h"

void ESP32Utils::reboot() {
    esp_restart();
}

uint32_t ESP32Utils::chipID() {
    uint64_t _chipmacid = 0LL;
    esp_efuse_mac_get_default((uint8_t*) (&_chipmacid));
    const uint64_t res {_chipmacid>>24};
    return static_cast<uint32_t>(res);
}

esp_reset_reason_t ESP32Utils::rebootReason() {
    return esp_reset_reason();
}

uint32_t ESP32Utils::millis() {
    static const uint64_t msFactor {1000ULL};
    return static_cast<uint32_t>(esp_timer_get_time()/msFactor);
}