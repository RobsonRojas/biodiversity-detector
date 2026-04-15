/*
 * Copyright (C) 2026 Robson Rojas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "inference_engine.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "model_data.hpp"
#include <cstring>
#include <algorithm>
#include <cmath>

static const char* TAG = "INFERENCE_ENGINE";

InferenceEngine::InferenceEngine()
    : initialized_(false),
      tensor_arena_(nullptr, heap_caps_free),
      model_(nullptr),
      resolver_(nullptr),
      interpreter_(nullptr),
      input_tensor_(nullptr) {}

InferenceEngine::~InferenceEngine() {}

esp_err_t InferenceEngine::init() {
    if (initialized_) return ESP_OK;

    // Allocate Tensor Arena in PSRAM for future use
    uint8_t* arena = (uint8_t*)heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (arena == nullptr) {
        ESP_LOGW(TAG, "Allocation in PSRAM failed. Falling back to internal SRAM (Tight!)");
        arena = (uint8_t*)malloc(kTensorArenaSize);
    }
    
    if (arena == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate %zu bytes for tensor arena.", kTensorArenaSize);
        return ESP_ERR_NO_MEM;
    }
    tensor_arena_.reset(arena);

    ESP_LOGI(TAG, "Initialized successfully. Arena size: %zu bytes", kTensorArenaSize);
    initialized_ = true;
    return ESP_OK;
}

esp_err_t InferenceEngine::classify(const float* signal, size_t signal_len,
                                    std::string& result_label, float& confidence) {
    if (!initialized_) return ESP_ERR_INVALID_STATE;
    
    // Simplified inference stub - returns mock results based on input characteristics
    if (signal_len == 0) {
        result_label = "Background";
        confidence = 0.5f;
        return ESP_OK;
    }

    // Calculate simple energy metric from signal
    float energy = 0.0f;
    float max_val = 0.0f;
    
    for (size_t i = 0; i < signal_len; i++) {
        energy += signal[i] * signal[i];
        max_val = std::max(max_val, std::abs(signal[i]));
    }
    energy = std::sqrt(energy / signal_len);

    uint64_t start_us = esp_timer_get_time();
    
    // Mock classification based on energy levels
    if (energy > 0.8f) {
        result_label = "Chainsaw";
        confidence = std::min(0.95f, 0.5f + energy * 0.5f);
    } else if (energy > 0.5f) {
        result_label = "Forest Activity";
        confidence = 0.7f;
    } else {
        result_label = "Background";
        confidence = 0.6f;
    }
    
    uint64_t end_us = esp_timer_get_time();
    ESP_LOGD(TAG, "Mock inference took %llu us. Result: %s (%.2f)", 
             (end_us - start_us), result_label.c_str(), confidence);
    
    return ESP_OK;
}
