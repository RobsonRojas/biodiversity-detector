/*
 * Copyright (C) 2026 Robson Rojas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"

/**
 * @brief Class to monitor battery voltage via ADC.
 */
class BatteryMonitor {
public:
    BatteryMonitor();
    ~BatteryMonitor();

    /**
     * @brief Initialize the ADC for battery sensing
     * @param channel ADC channel to use (e.g., ADC_CHANNEL_0 for GPIO 1 on S3)
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t init(adc_channel_t channel);

    /**
     * @brief Get current battery percentage (0-100)
     * @return int Percentage
     */
    int get_battery_level();

private:
    adc_oneshot_unit_handle_t adc_handle;
    adc_channel_t adc_channel;
    bool initialized;
};
