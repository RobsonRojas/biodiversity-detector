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
