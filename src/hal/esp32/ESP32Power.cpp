#include "../../utils/compat.hpp"
#include "ESP32Power.hpp"

#ifdef ESP_PLATFORM
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "esp_log.h"
#endif

namespace guardian::hal {

#ifdef ESP_PLATFORM
static const char* TAG = "HAL_POWER";
#endif

void ESP32Power::setup_deep_sleep() {
#ifdef ESP_PLATFORM
    ESP_LOGI(TAG, "Configuring RTC peripherals for Deep Sleep...");
    // Keep internal pull-ups if necessary or isolate pins
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
#endif
}

void ESP32Power::configure_wake_on_gpio(int pin) {
#ifdef ESP_PLATFORM
    ESP_LOGI(TAG, "Enabling wake-on-GPIO on pin: %d", pin);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)pin, 1); // Wake on high level
#endif
}

void ESP32Power::enter_deep_sleep() {
#ifdef ESP_PLATFORM
    ESP_LOGI(TAG, "Entering Deep Sleep now...");
    esp_deep_sleep_start();
#endif
}

} // namespace guardian::hal
