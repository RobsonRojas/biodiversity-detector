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

#include "audio_capture.hpp"
#include "esp_log.h"

static const char *TAG = "AUDIO_CAPTURE";

// pins for ESP32-S3-DevKitC (standard)
#define I2S_BCK_IO  (GPIO_NUM_42)
#define I2S_WS_IO   (GPIO_NUM_2)
#define I2S_DI_IO   (GPIO_NUM_41)

AudioCapture::AudioCapture() {}

AudioCapture::~AudioCapture() {
    ll_driver.stop();
}

esp_err_t AudioCapture::init(uint32_t sample_rate) {
    i2s_ll_config_t config = {};
    config.sample_rate = sample_rate;
    config.bck_io = I2S_BCK_IO;
    config.ws_io = I2S_WS_IO;
    config.din_io = I2S_DI_IO;

    esp_err_t ret = ll_driver.init(config, &ring_buffer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize low-level I2S driver");
        return ret;
    }

    ret = ll_driver.start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start low-level I2S driver");
        return ret;
    }

    ESP_LOGI(TAG, "Audio Capture initialized with Low-Level Zero-Copy Driver at %lu Hz", sample_rate);
    return ESP_OK;
}

const void* AudioCapture::get_next_buffer() {
    return ring_buffer.get_ready_segment_ptr();
}

const SegmentMetadata* AudioCapture::get_next_metadata() {
    return ring_buffer.get_ready_segment_metadata();
}

void AudioCapture::release_buffer() {
    ring_buffer.release_segment();
}

int AudioCapture::get_occupancy() {
    return ring_buffer.get_occupancy();
}
