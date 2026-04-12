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

#include <stddef.h>
#include <stdint.h>
#include "esp_heap_caps.h"
#include <memory>
#include "memory_helpers.hpp"

/**
 * @brief Sliding window buffer for audio samples, optimized for AI inference.
 * Allocates memory in PSRAM to handle large windows.
 */
class AudioBuffer {
public:
    /**
     * @brief Construct an AudioBuffer
     * @param capacity Total number of samples (e.g., 2 seconds * sample_rate)
     */
    AudioBuffer(size_t capacity);
    ~AudioBuffer();

    /**
     * @brief Add new samples to the end of the buffer
     * @param samples Pointer to new samples
     * @param count Number of samples to add
     */
    void push(const float* samples, size_t count);

    /**
     * @brief Get a pointer to the current full window
     * @return float* Pointer to raw samples (ordered from oldest to newest)
     */
    const float* get_window() const { return samples_.get(); }

    /**
     * @brief Check if the buffer is full (has reached capacity)
     */
    bool is_ready() const { return count_ >= capacity_; }

    size_t get_capacity() const { return capacity_; }

private:
    memory::DmaUniquePtr<float> samples_;
    size_t capacity_;
    size_t count_;
};
