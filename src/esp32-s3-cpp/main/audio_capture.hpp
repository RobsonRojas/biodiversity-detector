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
