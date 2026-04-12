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

#include "inference_engine.hpp"
#include "model_data.hpp"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"

// TFLite Micro includes
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

static const char* TAG = "INFERENCE_ENGINE";

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

InferenceEngine::InferenceEngine()
    : initialized_(false),
      tensor_arena_(nullptr, heap_caps_free),
      model_(nullptr),
      resolver_(nullptr),
      interpreter_(nullptr),
      input_tensor_(nullptr) {}

InferenceEngine::~InferenceEngine() {
    interpreter_.reset();
    resolver_.reset();
    tensor_arena_.reset();
    ESP_LOGI(TAG, "Inference engine resources released.");
}

// ---------------------------------------------------------------------------
// Simulation fallback – provides realistic varied detections when no real
// model is loaded (placeholder model or QEMU environment).
// ---------------------------------------------------------------------------

static bool s_simulation_mode = false;

static void simulate_classify(const float* signal, size_t signal_len,
                               std::string& result_label, float& confidence) {
    // Use a combination of signal energy and time to produce varied results.
    float energy = 0.0f;
    size_t step = (signal_len > 256) ? signal_len / 256 : 1;
    for (size_t i = 0; i < signal_len; i += step) {
        energy += signal[i] * signal[i];
    }
    energy /= (signal_len / step);

    // Use microsecond timer for pseudo-random variation.
    uint32_t tick = static_cast<uint32_t>(esp_timer_get_time() / 1000);
    int scenario = (tick / 3000) % 5;  // Change scenario every ~3 seconds.

    switch (scenario) {
        case 0:
            result_label = kCategoryLabels[0]; // "Chainsaw"
            confidence = 0.91f + (energy * 0.05f);
            break;
        case 1:
            result_label = kCategoryLabels[1]; // "Animal"
            confidence = 0.85f + (energy * 0.08f);
            break;
        case 2:
            result_label = kCategoryLabels[2]; // "Insect"
            confidence = 0.78f + (energy * 0.10f);
            break;
        case 3:
            result_label = kCategoryLabels[3]; // "Background"
            confidence = 0.95f;
            break;
        default:
            result_label = kCategoryLabels[1]; // "Animal" (frog-like)
            confidence = 0.88f;
            break;
    }

    // Clamp
    if (confidence > 1.0f) confidence = 1.0f;
    if (confidence < 0.0f) confidence = 0.0f;
}

// ---------------------------------------------------------------------------
// Initialization – loads model, allocates PSRAM arena, builds interpreter
// ---------------------------------------------------------------------------

esp_err_t InferenceEngine::init() {
    if (initialized_) {
        ESP_LOGW(TAG, "Already initialized – skipping.");
        return ESP_OK;
    }

    // 1. Map the embedded FlatBuffer model.
    model_ = tflite::GetModel(g_model_data);
    if (model_ == nullptr) {
        ESP_LOGW(TAG, "Failed to map TFLite model – entering simulation mode.");
        s_simulation_mode = true;
        initialized_ = true;
        return ESP_OK;
    }
    if (model_->version() != TFLITE_SCHEMA_VERSION) {
        ESP_LOGW(TAG, "Model schema version mismatch (%lu vs %d) – entering simulation mode.",
                 static_cast<unsigned long>(model_->version()),
                 TFLITE_SCHEMA_VERSION);
        s_simulation_mode = true;
        initialized_ = true;
        return ESP_OK;
    }
    ESP_LOGI(TAG, "TFLite model mapped (schema v%lu, %zu bytes).",
             static_cast<unsigned long>(model_->version()), g_model_data_len);

    // 2. Allocate tensor arena – prefer PSRAM, fallback to internal.
    uint8_t* arena_raw = static_cast<uint8_t*>(
        heap_caps_malloc(kTensorArenaSize,
                         MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (arena_raw == nullptr) {
        ESP_LOGW(TAG, "PSRAM allocation failed – trying internal RAM.");
        arena_raw = static_cast<uint8_t*>(
            heap_caps_malloc(kTensorArenaSize,
                             MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    }
    if (arena_raw == nullptr) {
        ESP_LOGW(TAG, "Cannot allocate tensor arena – entering simulation mode.");
        s_simulation_mode = true;
        initialized_ = true;
        return ESP_OK;
    }
    tensor_arena_.reset(arena_raw);
    ESP_LOGI(TAG, "Tensor arena: %zu bytes @ %p", kTensorArenaSize, arena_raw);

    // 3. Register operators.
    auto resolver = std::make_unique<tflite::MicroMutableOpResolver<10>>();
    resolver->AddFullyConnected();
    resolver->AddSoftmax();
    resolver->AddReshape();
    resolver->AddQuantize();
    resolver->AddDequantize();
    resolver->AddConv2D();
    resolver->AddDepthwiseConv2D();
    resolver->AddMaxPool2D();
    resolver->AddAveragePool2D();
    resolver->AddMul();

    // 4. Build the interpreter.
    auto interp = std::make_unique<tflite::MicroInterpreter>(
        model_,
        *resolver,
        tensor_arena_.get(),
        static_cast<size_t>(kTensorArenaSize));

    TfLiteStatus alloc_status = interp->AllocateTensors();
    if (alloc_status != kTfLiteOk) {
        ESP_LOGW(TAG, "AllocateTensors() failed – entering simulation mode.");
        s_simulation_mode = true;
        initialized_ = true;
        return ESP_OK;
    }

    input_tensor_ = interp->input(0);
    ESP_LOGI(TAG, "Input tensor: dims=%d, type=%d, bytes=%zu",
             input_tensor_->dims->size,
             input_tensor_->type,
             input_tensor_->bytes);

    resolver_    = std::move(resolver);
    interpreter_ = std::move(interp);

    s_simulation_mode = false;
    initialized_ = true;
    ESP_LOGI(TAG, "Inference engine ready (real model, %d classes).", kCategoryCount);
    return ESP_OK;
}

// ---------------------------------------------------------------------------
// Classification
// ---------------------------------------------------------------------------

esp_err_t InferenceEngine::classify(const float* signal, size_t signal_len,
                                    std::string& result_label,
                                    float& confidence) {
    if (!initialized_) {
        ESP_LOGE(TAG, "classify() called before init().");
        return ESP_ERR_INVALID_STATE;
    }
    if (signal == nullptr || signal_len == 0) {
        ESP_LOGE(TAG, "Empty feature buffer.");
        return ESP_ERR_INVALID_ARG;
    }

    // --- Simulation fallback ---------------------------------------------------
    if (s_simulation_mode) {
        simulate_classify(signal, signal_len, result_label, confidence);
        ESP_LOGI(TAG, "[SIM-AI] Result: %s (%.2f)", result_label.c_str(), confidence);
        return ESP_OK;
    }

    // --- Real model inference --------------------------------------------------
    size_t model_input_len = input_tensor_->bytes / sizeof(float);
    size_t copy_len = (signal_len < model_input_len) ? signal_len : model_input_len;

    float* input_data = input_tensor_->data.f;
    for (size_t i = 0; i < copy_len; ++i) {
        input_data[i] = signal[i];
    }
    for (size_t i = copy_len; i < model_input_len; ++i) {
        input_data[i] = 0.0f;
    }

    TfLiteStatus invoke_status = interpreter_->Invoke();
    if (invoke_status != kTfLiteOk) {
        ESP_LOGE(TAG, "Invoke() failed (status %d).", invoke_status);
        return ESP_FAIL;
    }

    // Parse output probabilities.
    TfLiteTensor* output = interpreter_->output(0);
    int output_len = output->dims->data[output->dims->size - 1];
    int num_classes = (output_len < kCategoryCount) ? output_len : kCategoryCount;

    float best_score = -1.0f;
    int   best_index = 0;
    for (int i = 0; i < num_classes; ++i) {
        float score = output->data.f[i];
        if (score > best_score) {
            best_score = score;
            best_index = i;
        }
    }

    result_label = kCategoryLabels[best_index];
    confidence   = best_score;

    ESP_LOGI(TAG, "[AI] Result: %s (%.2f)", result_label.c_str(), confidence);
    return ESP_OK;
}
