/*
 * Copyright (C) 2026 Robson Rojas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "dsp_analyzer.hpp"
#include "esp_log.h"
#include <string.h>

static const char* TAG = "DSP_ANALYZER_MOCK";

DspAnalyzer::DspAnalyzer() : n_fft(0) {}

esp_err_t DspAnalyzer::init(int fft_size) {
    n_fft = fft_size;
    ESP_LOGI(TAG, "Initialized in MOCK mode (No esp-dsp). Size: %d", n_fft);
    return ESP_OK;
}

void DspAnalyzer::analyze(const int32_t* input, float& mechanical_energy, float& biological_energy) {
    // Generate some simulated energy values
    mechanical_energy = 0.5f;
    biological_energy = 0.2f;
}
