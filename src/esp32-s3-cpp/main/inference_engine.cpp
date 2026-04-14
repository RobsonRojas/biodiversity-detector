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

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

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

    // 1. Load the model
    model_ = tflite::GetModel(g_model_data);
    if (model_->version() != TFLITE_SCHEMA_VERSION) {
        ESP_LOGE(TAG, "Model schema version %ld is not supported (Expected: %d).",
                 model_->version(), TFLITE_SCHEMA_VERSION);
        return ESP_FAIL;
    }

    // 2. Allocate Tensor Arena in PSRAM
    // We use a custom deleter for the unique_ptr because heap_caps_free must be used.
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

    // 3. Set up the Op Resolver (Using AllOps for safety/ease of use)
    resolver_ = std::make_unique<tflite::AllOpsResolver>();

    // 4. Instantiate the Interpreter
    interpreter_ = std::make_unique<tflite::MicroInterpreter>(
        model_, *resolver_, arena, kTensorArenaSize);

    // 5. Allocate Tensors
    if (interpreter_->AllocateTensors() != kTfLiteOk) {
        ESP_LOGE(TAG, "AllocateTensors() failed.");
        return ESP_FAIL;
    }

    // 6. Cache pointers
    input_tensor_ = interpreter_->input(0);

    ESP_LOGI(TAG, "Initialized successfully. Arena size: %zu bytes", kTensorArenaSize);
    initialized_ = true;
    return ESP_OK;
}

esp_err_t InferenceEngine::classify(const float* signal, size_t signal_len,
                                    std::string& result_label, float& confidence) {
    if (!initialized_) return ESP_ERR_INVALID_STATE;
    
    // 1. Copy features into the input tensor
    // Edge Impulse models usually expect quantized int8 or float32.
    // Based on hpp, we assume float32 input.
    if (input_tensor_->bytes / sizeof(float) < signal_len) {
        ESP_LOGE(TAG, "Signal length %zu exceeds input tensor capacity %zu", 
                 signal_len, input_tensor_->bytes / sizeof(float));
        return ESP_ERR_INVALID_ARG;
    }
    
    float* input_data = input_tensor_->data.f;
    for (size_t i = 0; i < signal_len; i++) {
        input_data[i] = signal[i];
    }

    // 2. Run Inference
    uint64_t start_us = esp_timer_get_time();
    if (interpreter_->Invoke() != kTfLiteOk) {
        ESP_LOGE(TAG, "Invoke() failed");
        return ESP_FAIL;
    }
    uint64_t end_us = esp_timer_get_time();

    // 3. Process the output tensor
    TfLiteTensor* output_tensor = interpreter_->output(0);
    float* output_data = output_tensor->data.f;
    
    int best_index = 0;
    float max_score = -1.0f;
    
    for (int i = 0; i < kCategoryCount; i++) {
        if (output_data[i] > max_score) {
            max_score = output_data[i];
            best_index = i;
        }
    }

    result_label = kCategoryLabels[best_index];
    confidence = max_score;
    
    ESP_LOGD(TAG, "Inference took %llu us. Win: %s (%.2f)", 
             (end_us - start_us), result_label.c_str(), confidence);
    
    return ESP_OK;
}
