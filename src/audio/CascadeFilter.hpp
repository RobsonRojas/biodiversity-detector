#pragma once

#include <vector>
#include <cstdint>
#include <cmath>

namespace guardian::audio {

enum class CascadeState {
    QUIET,      // Deep Sleep / Low Power RMS monitor
    DSP_VERIFY, // DSP Frequency filtering
    AI_INFERENCE, // Full AI classification
    ALERT       // Alerting
};

class CascadeFilter {
public:
    CascadeFilter(float rms_threshold);

    /**
     * @brief Calculate RMS of a buffer and determine if Stage 1 threshold is met.
     */
    bool check_rms_threshold(const std::vector<int16_t>& samples);

    /**
     * @brief Perform frequency band analysis (Stage 2).
     * @return true if pattern matches expected chainsaw/harmonics.
     */
    bool verify_frequency_pattern(const std::vector<int16_t>& samples);

private:
    float _rms_threshold;
};

} // namespace guardian::audio
