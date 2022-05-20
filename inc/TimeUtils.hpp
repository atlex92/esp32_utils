#pragma once

#include "stdint.h"

namespace TimeUtils {
    bool isPeriodPassed(const uint32_t start, const uint32_t period);
    uint32_t nowMs();
};