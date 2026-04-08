#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace guardian::ai {

struct DetectionResult {
    std::string label;
    float confidence;
};

class EdgeImpulseWrapper {
public:
    EdgeImpulseWrapper();

    /**
     * @brief Initialize the TinyML engine.
     */
    bool init();

    /**
     * @brief Run inference on a window of audio samples.
     * @param samples Pointer to raw 16-bit audio samples.
     * @param count Number of samples.
     * @return List of classified results.
     */
    std::vector<DetectionResult> classify(const int16_t* samples, size_t count);

private:
    /**
     * @brief Extract spectrogram-like features from audio.
     */
    std::vector<float> extract_features(const int16_t* samples, size_t count);
};

} // namespace guardian::ai
