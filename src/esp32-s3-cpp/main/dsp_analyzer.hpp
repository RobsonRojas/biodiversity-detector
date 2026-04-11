#pragma once

#include "dsps_fft2r.h"
#include "esp_err.h"
#include <vector>
#include <memory>

/**
 * @brief Class to analyze audio data using FFT.
 * Supports multi-band energy calculation for mechanical (chainsaw) 
 * and biological (biodiversity) sound signatures.
 */
class DspAnalyzer {
public:
    DspAnalyzer();
    ~DspAnalyzer() = default;

    /**
     * @brief Initialize the FFT buffers
     * @param fft_size Size of FFT (must be power of 2)
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t init(int fft_size);

    /**
     * @brief Process audio block and calculate energy in specific bands
     * @param input Input audio buffer (samples will be normalized)
     * @param mechanical_energy Output energy for chainsaw frequency range (100Hz-2kHz)
     * @param biological_energy Output energy for biodiversity frequency range (2kHz-20kHz)
     */
    void analyze(const int32_t* input, float& mechanical_energy, float& biological_energy);

private:
    int n_fft;
    std::unique_ptr<float[]> window;
    std::unique_ptr<float[]> fft_input;
    std::unique_ptr<float[]> fft_output;
};
