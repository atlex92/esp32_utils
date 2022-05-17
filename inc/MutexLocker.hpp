#pragma once

#include "freertos/semphr.h"

struct MutexLocker {

    explicit MutexLocker(SemaphoreHandle_t mutex) : _mutex{mutex} {
        xSemaphoreTakeRecursive(_mutex, portMAX_DELAY);
    }

    ~MutexLocker() {
        xSemaphoreGiveRecursive(_mutex);
    }
    private:
        SemaphoreHandle_t _mutex;
};