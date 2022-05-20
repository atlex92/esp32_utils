#pragma once
#include "stdint.h"

class BatteryMeter  {

    public:
        explicit BatteryMeter(const uint16_t dischargedVoltage, const uint16_t fullVoltage)
            :   _dischargedVoltage{dischargedVoltage},
            _fullVoltage{fullVoltage} {

        }
        uint8_t chargePercent(const uint16_t voltage){
    
            float percent = 100.0 * (static_cast<float>(voltage) - _dischargedVoltage)/(static_cast<float>(_fullVoltage) - _dischargedVoltage);

            if (percent > 100.0) {
                percent = 100.0;
            }

            if (percent < 0.0) {
                percent = 0.0;
            }

            return static_cast<uint8_t>(percent + 0.5);
        }
    private:
        uint16_t _dischargedVoltage;
        uint16_t _fullVoltage;
};