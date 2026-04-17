#include "inference_engine.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "model_data.hpp"

// TFLite Micro headers
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include <cstring>
#include <algorithm>
#include <cmath>

static const char* TAG = "INFERENCE_ENGINE";

InferenceEngine::InferenceEngine()
    : initialized_(false),
      tensor_arena_(nullptr, heap_caps_free),
      model_(nullptr),
      interpreter_(nullptr),
      input_tensor_(nullptr),
      resolver_(nullptr),
      input_scale_(0.0f),
      input_zero_point_(0) {}

InferenceEngine::~InferenceEngine() {
    // Note: interpreter uses a static instance for simplicity in this version,
    // so no manual deletion is required here unless allocated via new.
}

esp_err_t InferenceEngine::init() {
    if (initialized_) return ESP_OK;

    // 1. Load Model from binary array
    model_ = tflite::GetModel(g_model_data);
    if (model_->version() != TFLITE_SCHEMA_VERSION) {
        ESP_LOGE(TAG, "Model schema version %d not supported (expected %d)",
                 (int)model_->version(), TFLITE_SCHEMA_VERSION);
        return ESP_FAIL;
    }

    // 2. Allocate Tensor Arena in PSRAM
    uint8_t* arena = (uint8_t*)heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (arena == nullptr) {
        ESP_LOGW(TAG, "Allocation in PSRAM failed. Falling back to internal SRAM");
        arena = (uint8_t*)malloc(kTensorArenaSize);
    }
    
    if (arena == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate %zu bytes for tensor arena.", kTensorArenaSize);
        return ESP_ERR_NO_MEM;
    }
    tensor_arena_.reset(arena);

    // 3. Setup Resolver and Register Operations
    // Using MicroMutableOpResolver instead of AllOpsResolver to save flash space.
    auto micro_resolver = std::make_unique<tflite::MicroMutableOpResolver<15>>();
    micro_resolver->AddAdd();
    micro_resolver->AddAveragePool2D();
    micro_resolver->AddConv2D();
    micro_resolver->AddDepthwiseConv2D();
    micro_resolver->AddDequantize();
    micro_resolver->AddFullyConnected();
    micro_resolver->AddMaxPool2D();
    micro_resolver->AddSoftmax();
    micro_resolver->AddReshape();
    micro_resolver->AddQuantize();
    micro_resolver->AddSum();
    
    resolver_ = std::move(micro_resolver);
    
    // 4. Initialize Interpreter
    static tflite::MicroInterpreter static_interpreter(
        model_, *resolver_, tensor_arena_.get(), kTensorArenaSize);
    interpreter_ = &static_interpreter;

    // 5. Allocate Tensors
    if (interpreter_->AllocateTensors() != kTfLiteOk) {
        ESP_LOGE(TAG, "AllocateTensors() failed");
        return ESP_FAIL;
    }

    // 6. Get Input Tensor and Quantization Parameters
    input_tensor_ = interpreter_->input(0);
    if (input_tensor_->type == kTfLiteInt8) {
        input_scale_ = input_tensor_->params.scale;
        input_zero_point_ = input_tensor_->params.zero_point;
        ESP_LOGI(TAG, "Quantized model detected. Scale: %f, ZeroPoint: %d", input_scale_, (int)input_zero_point_);
    }

    ESP_LOGI(TAG, "Initialized successfully. Arena size: %zu bytes", kTensorArenaSize);
    initialized_ = true;
    return ESP_OK;
}

esp_err_t InferenceEngine::classify(const float* signal, size_t signal_len,
                                    std::string& result_label, float& confidence) {
    if (!initialized_) return ESP_ERR_INVALID_STATE;
    
    if (signal_len == 0 || signal == nullptr) {
        result_label = "Background";
        confidence = 0.0f;
        return ESP_OK;
    }

    // 1. Preprocess and Quantize Input
    size_t bytes_to_copy = std::min(signal_len, input_tensor_->bytes);
    int8_t* input_data = input_tensor_->data.int8;

    for (size_t i = 0; i < bytes_to_copy; i++) {
        // Quantization formula: q = (f / scale) + zero_point
        float quantized_val = (signal[i] / input_scale_) + input_zero_point_;
        input_data[i] = (int8_t)std::clamp(quantized_val, -128.0f, 127.0f);
    }

    uint64_t start_us = esp_timer_get_time();
    
    // 2. Run Inference
    if (interpreter_->Invoke() != kTfLiteOk) {
        ESP_LOGE(TAG, "Invoke() failed");
        return ESP_FAIL;
    }
    
    // 3. Process Output
    TfLiteTensor* output = interpreter_->output(0);
    int8_t* output_data = output->data.int8;
    float output_scale = output->params.scale;
    int32_t output_zero_point = output->params.zero_point;

    int top_index = 0;
    float max_score = -1.0f;

    for (int i = 0; i < kCategoryCount; i++) {
        // De-quantization: f = (q - zero_point) * scale
        float score = (output_data[i] - output_zero_point) * output_scale;
        if (score > max_score) {
            max_score = score;
            top_index = i;
        }
    }

    uint64_t end_us = esp_timer_get_time();
    
    result_label = kCategoryLabels[top_index];
    confidence = max_score;

    ESP_LOGD(TAG, "Inference took %llu us. Result: %s (%.2f)", 
             (end_us - start_us), result_label.c_str(), confidence);
    
    return ESP_OK;
}
