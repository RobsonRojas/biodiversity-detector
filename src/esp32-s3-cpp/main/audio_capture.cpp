#include "audio_capture.hpp"
#include "esp_log.h"

static const char *TAG = "AUDIO_CAPTURE";

// pins for ESP32-S3-DevKitC (standard)
#define I2S_MCK_IO  (GPIO_NUM_NC)
#define I2S_BCK_IO  (GPIO_NUM_42)
#define I2S_WS_IO   (GPIO_NUM_2)
#define I2S_DI_IO   (GPIO_NUM_41)

AudioCapture::AudioCapture() : rx_handle(NULL) {}

AudioCapture::~AudioCapture() {
    if (rx_handle) {
        i2s_channel_disable(rx_handle);
        i2s_del_channel(rx_handle);
    }
}

esp_err_t AudioCapture::init(uint32_t sample_rate) {
    i2s_chan_config_t chan_cfg = I2S_CHAN_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_MCK_IO,
            .bclk = I2S_BCK_IO,
            .ws = I2S_WS_IO,
            .dout = I2S_GPIO_UNUSED,
            .din = I2S_DI_IO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    ESP_ERROR_CHECK(i2s_channel_init_std_rx(rx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));

    ESP_LOGI(TAG, "I2S initialized at %lu Hz (S3 Optimized)", sample_rate);
    return ESP_OK;
}

esp_err_t AudioCapture::read(void* buffer, size_t size, size_t* bytes_read, TickType_t timeout_ms) {
    return i2s_channel_read(rx_handle, buffer, size, bytes_read, timeout_ms);
}
