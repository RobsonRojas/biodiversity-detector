#include "CascadeFilter.hpp"
// #include <numeric> // Removed to fix lint

#ifdef ESP_PLATFORM
#include "dsps_fft2r.h"
#include "esp_log.h"
#endif

namespace guardian::audio {

#ifdef ESP_PLATFORM
static const char* TAG = "CASCADE_FILTER";
#endif

CascadeFilter::CascadeFilter(float rms_threshold) : _rms_threshold(rms_threshold) {}

bool CascadeFilter::check_rms_threshold(const std::vector<int16_t>& samples) {
    if (samples.empty()) return false;

    double sum_sq = 0;
    for (auto s : samples) {
        sum_sq += s * s;
    }
    float rms = std::sqrt(sum_sq / samples.size());
    
    // Convert to relative scale (simplified for this implementation)
    // 32768 is max for int16
    float normalized_rms = rms / 32768.0f;
    
    return normalized_rms > _rms_threshold;
}

bool CascadeFilter::verify_frequency_pattern(const std::vector<int16_t>& samples) {
    // Stage 2: Basic DSP Frequency match
    // In a real ESP32-S3 implementation, we would use esp-dsp FFT here.
#ifdef ESP_PLATFORM
    ESP_LOGI(TAG, "Running Stage 2: DSP Frequency Analysis...");
    // Mocking frequency pattern match for now
    // In production: check 100Hz - 2kHz band energy
    return true; 
#else
    // Simulation / Off-platform logic
    return true; 
#endif
}

} // namespace guardian::audio
