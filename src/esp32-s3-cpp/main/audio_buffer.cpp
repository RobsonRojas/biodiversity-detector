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

#include "audio_buffer.hpp"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "AUDIO_BUFFER";

AudioBuffer::AudioBuffer(size_t capacity) : capacity_(capacity), count_(0) {
    // Allocate in SPIRAM (PSRAM) using RAII
    samples_ = memory::make_dma_unique<float>(capacity, MALLOC_CAP_SPIRAM);
    if (!samples_) {
        ESP_LOGE(TAG, "Failed to allocate %zu samples in PSRAM!", capacity);
    } else {
        memset(samples_.get(), 0, capacity * sizeof(float));
        ESP_LOGI(TAG, "Allocated %zu samples in PSRAM.", capacity);
    }
}

AudioBuffer::~AudioBuffer() = default;

void AudioBuffer::push(const float* samples, size_t count) {
    if (!samples_) return;

    if (count >= capacity_) {
        // Direct copy of the last part
        memcpy(samples_.get(), samples + count - capacity_, capacity_ * sizeof(float));
        count_ = capacity_;
    } else {
        // Shift existing samples left
        memmove(samples_.get(), samples_.get() + count, (capacity_ - count) * sizeof(float));
        // Copy new samples to the end
        memcpy(samples_.get() + capacity_ - count, samples, count * sizeof(float));
        
        if (count_ < capacity_) {
            count_ += count;
            if (count_ > capacity_) count_ = capacity_;
        }
    }
}
