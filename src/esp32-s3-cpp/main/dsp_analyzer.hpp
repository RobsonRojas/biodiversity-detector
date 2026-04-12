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
