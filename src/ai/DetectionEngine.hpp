#pragma once

#include <system_error>
#include <vector>
#include <string>
#include "../utils/compat.hpp"

namespace guardian::ai {

class DetectionEngine {
public:
    DetectionEngine();
    ~DetectionEngine();

    /**
     * @brief Initializes TFLite Micro and loads the model.
     * @param model_path Path to the .tflite model.
     * @return Result or error.
     */
    guardian::expected<void, std::error_code> initialize(const std::string& model_path);

    /**
     * @brief Performs inference on a 1-second audio frame.
     * @param audio_frame 16,000 samples.
     * @return Confidence score [0.0 - 1.0].
     */
    guardian::expected<float, std::error_code> detect_motoserra(guardian::span<const int32_t> audio_frame);

private:
    bool initialized_;
    // TFLite Micro specific pointers would go here
    // For this simulation/demo we use placeholders
};

} // namespace guardian::ai
