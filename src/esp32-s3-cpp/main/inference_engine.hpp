#pragma once

#include "esp_err.h"
#include <vector>
#include <string>
#include <memory>

// Forward-declare TFLite Micro types to keep the header SDK-free.
namespace tflite {
    class MicroInterpreter;
    class MicroMutableOpResolver;
    class Model;
}  // namespace tflite
struct TfLiteTensor;

/**
 * @brief Real AI inference engine using TensorFlow Lite Micro.
 *
 * Loads an embedded TFLite model, allocates the tensor arena in PSRAM,
 * and runs multi-class classification on normalized audio features.
 */
class InferenceEngine {
public:
    InferenceEngine();
    ~InferenceEngine();

    // Non-copyable, non-movable (owns hardware-aligned memory).
    InferenceEngine(const InferenceEngine&) = delete;
    InferenceEngine& operator=(const InferenceEngine&) = delete;

    /**
     * @brief Initialize the TFLite interpreter and allocate tensors.
     * @return ESP_OK on success, or an error code if the model fails to load.
     */
    esp_err_t init();

    /**
     * @brief Run inference on normalized audio features.
     * @param signal      Pointer to float array of normalized audio features.
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

    // TFLite Micro objects (opaque pointers; impl owns lifetime).
    const tflite::Model* model_;
    std::unique_ptr<tflite::MicroMutableOpResolver> resolver_;
    std::unique_ptr<tflite::MicroInterpreter>       interpreter_;
    TfLiteTensor* input_tensor_;
};
