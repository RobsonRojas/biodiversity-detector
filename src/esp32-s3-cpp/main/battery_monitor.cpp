#include "battery_monitor.hpp"
#include "esp_log.h"

static const char *TAG = "BATTERY_MONITOR";

BatteryMonitor::BatteryMonitor() : adc_handle(NULL), adc_channel(ADC_CHANNEL_0), initialized(false) {}

BatteryMonitor::~BatteryMonitor() {
    if (initialized) {
        adc_oneshot_del_unit(adc_handle);
    }
}

esp_err_t BatteryMonitor::init(adc_channel_t channel) {
    adc_channel = channel;
    
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12, // For S3, 12dB allows reading up to ~3.1V raw
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, adc_channel, &config));

    initialized = true;
    ESP_LOGI(TAG, "Battery Monitoring initialized on ADC Channel %d", channel);
    return ESP_OK;
}

int BatteryMonitor::get_battery_level() {
    if (!initialized) return 0;

    int adc_raw;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, adc_channel, &adc_raw));

    // Simple mapping for demonstration (assuming 12-bit ADC, 0-4095)
    // In real hardware, we translate to voltage and then compare to a discharge curve.
    // Assuming max safe input is 3.1V, and S3 ADC is 12-bit.
    // If we have a 1:1 voltage divider, 4.2V battery becomes 2.1V at ADC.
    // 2.1V / 3.1V * 4095 = ~2772
    // 3.3V battery (cut-off) becomes 1.65V at ADC.
    // 1.65V / 3.1V * 4095 = ~2175
    
    if (adc_raw >= 2772) return 100;
    if (adc_raw <= 2175) return 0;
    
    return (adc_raw - 2175) * 100 / (2772 - 2175);
}
