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

static const char* TAG = "INFERENCE_ENGINE_MOCK";

InferenceEngine::InferenceEngine()
    : initialized_(false),
      tensor_arena_(nullptr, free),
      model_(nullptr),
      resolver_(nullptr),
      interpreter_(nullptr),
      input_tensor_(nullptr) {}

InferenceEngine::~InferenceEngine() {}

esp_err_t InferenceEngine::init() {
    ESP_LOGI(TAG, "Initialized in MOCK mode (No TFLite).");
    initialized_ = true;
    return ESP_OK;
}

esp_err_t InferenceEngine::classify(const float* signal, size_t signal_len,
                                    std::string& result_label, float& confidence) {
    if (!initialized_) return ESP_ERR_INVALID_STATE;
    
    // Simulate periodic detections based on signal energy
    float energy = 0.0f;
    for (size_t i = 0; i < signal_len; ++i) energy += signal[i] * signal[i];
    
    result_label = "Background";
    confidence = 0.99f;
    
    if (energy > 10.0f) {
        result_label = "Chainsaw";
        confidence = 0.85f;
    }
    
    return ESP_OK;
}
