#include "I2SCapture.hpp"

#ifdef ESP_PLATFORM
#include "driver/i2s.h"
#include "esp_log.h"
#endif

namespace guardian::hal {

#ifdef ESP_PLATFORM
static const char* TAG = "HAL_I2S";
#define I2S_NUM I2S_NUM_0
#endif

void I2SCapture::init(uint32_t sample_rate) {
#ifdef ESP_PLATFORM
    ESP_LOGI(TAG, "Initializing I2S for audio capture (DMA)...");
    
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = sample_rate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);

    i2s_pin_config_t pin_config = {
        .bck_io_num = 14,
        .ws_io_num = 15,
        .data_out_num = -1,
        .data_in_num = 32
    };

    i2s_set_pin(I2S_NUM, &pin_config);
#endif
}

size_t I2SCapture::read(std::vector<int16_t>& buffer) {
    size_t bytes_read = 0;
#ifdef ESP_PLATFORM
    // ESP32 specifically requires reading into byte array then casting
    i2s_read(I2S_NUM, buffer.data(), buffer.size() * sizeof(int16_t), &bytes_read, portMAX_DELAY);
#endif
    return bytes_read / sizeof(int16_t);
}

void I2SCapture::deinit() {
#ifdef ESP_PLATFORM
    i2s_driver_uninstall(I2S_NUM);
#endif
}

} // namespace guardian::hal
