#pragma once

#include "i2s_low_level.hpp"
#include "shared_ring_buffer.hpp"

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

    // Zero-copy accessors
    const void* get_next_buffer();
    const SegmentMetadata* get_next_metadata();
    void release_buffer();
    int get_occupancy();

private:
    I2SLowLevel ll_driver;
    SharedRingBuffer ring_buffer;
};
