#pragma once

#include <stdint.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"
#include "esp_err.h"

/**
 * @brief Class to handle I2S audio capture from a digital microphone.
 * Optimzed for ESP32-S3 and high-sample rates for biodiversity monitoring.
 */
class AudioCapture {
public:
    AudioCapture();
    ~AudioCapture();

    /**
     * @brief Initialize the I2S driver
     * @param sample_rate Sample rate in Hz (e.g., 16000 or 44100)
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t init(uint32_t sample_rate);

    /**
     * @brief Read a block of audio data
     * @param buffer Pointer to the destination buffer
     * @param size Size in bytes
     * @param bytes_read Number of bytes actually read
     * @param timeout_ms Timeout for reading
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t read(void* buffer, size_t size, size_t* bytes_read, TickType_t timeout_ms);

private:
    i2s_chan_handle_t rx_handle;
};
