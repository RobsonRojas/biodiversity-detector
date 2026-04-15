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

#include "esp_err.h"
#include <vector>
#include <string>
#include <memory>
#include <cstddef>

/**
 * @brief Simplified AI inference engine (stub implementation).
 *
 * Allocates the tensor arena in PSRAM and provides classification interface.
 * This is a mock implementation for development purposes.
 */
class InferenceEngine {
public:
    static constexpr size_t kTensorArenaSize = 256 * 1024; // 256 KB
    static constexpr int kCategoryCount = 3;
    
    InferenceEngine();
    ~InferenceEngine();

    // Non-copyable, non-movable (owns hardware-aligned memory).
    InferenceEngine(const InferenceEngine&) = delete;
    InferenceEngine& operator=(const InferenceEngine&) = delete;

    /**
     * @brief Initialize the inference engine and allocate tensors.
     * @return ESP_OK on success, or an error code if allocation fails.
     */
    esp_err_t init();

    /**
     * @brief Run inference on audio signal.
     * @param signal      Pointer to float array of audio features.
     * @param signal_len  Number of elements in signal.
     * @param result_label [out] Name of the class with highest probability.
     * @param confidence   [out] Probability value (0.0 – 1.0) of the top class.
     * @return ESP_OK on success.
     */
    esp_err_t classify(const float* signal, size_t signal_len,
                       std::string& result_label, float& confidence);

private:
    bool initialized_;

    // PSRAM-backed arena managed via RAII.
    std::unique_ptr<uint8_t, void(*)(void*)> tensor_arena_;
    
    // Placeholder pointers (not used in stub implementation)
    void* model_;
    void* resolver_;
    void* interpreter_;
    void* input_tensor_;
};
