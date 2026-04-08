#pragma once
#include "../utils/compat.hpp"

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
    bool check_rms_threshold(guardian::span<const int16_t> samples);
    bool verify_frequency_pattern(guardian::span<const int16_t> samples);

private:
    float _rms_threshold;
};

} // namespace guardian::audio
