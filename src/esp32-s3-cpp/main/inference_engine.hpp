#pragma once

#include "esp_err.h"
#include <vector>
#include <string>

/**
 * @brief Class to manage AI Inference using Edge Impulse / TFLite Micro.
 */
class InferenceEngine {
public:
    InferenceEngine();
    ~InferenceEngine();

    /**
     * @brief Initialize the inference engine
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t init();

    /**
     * @brief Run inference on a block of audio data
     * @param signal Normalized audio features
     * @param result_label Output label of detected class
     * @param confidence Output confidence of detection
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t classify(const float* signal, size_t signal_len, std::string& result_label, float& confidence);

private:
    // This will eventually wrap the Edge Impulse / TFLite Micro SDK types
    bool initialized;
};
