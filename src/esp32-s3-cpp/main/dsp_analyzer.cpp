#include "dsp_analyzer.hpp"
#include "esp_log.h"
#include "dsps_view.h"
#include "dsps_wind.h"
#include <math.h>

static const char *TAG = "DSP_ANALYZER";

DspAnalyzer::DspAnalyzer() : n_fft(0), window(NULL), fft_input(NULL), fft_output(NULL) {}

DspAnalyzer::~DspAnalyzer() {
    if (window) free(window);
    if (fft_input) free(fft_input);
    if (fft_output) free(fft_output);
}

esp_err_t DspAnalyzer::init(int fft_size) {
    n_fft = fft_size;
    esp_err_t ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE); // Using default internal table
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Not possible to initialize FFT. Error = %i", ret);
        return ret;
    }

    window = (float*)malloc(n_fft * sizeof(float));
    fft_input = (float*)malloc(n_fft * 2 * sizeof(float)); // Complex input
    fft_output = (float*)malloc(n_fft * 2 * sizeof(float)); // Complex output

    dsps_wind_hann_f32(window, n_fft);

    ESP_LOGI(TAG, "DSP Analyzer initialized with FFT size %d", n_fft);
    return ESP_OK;
}

void DspAnalyzer::analyze(const int32_t* input, float& mechanical_energy, float& biological_energy) {
    // 1. Normalize and Apply Window
    for (int i = 0; i < n_fft; i++) {
        fft_input[i * 2 + 0] = (float)input[i] * window[i] / 2147483648.0f; // Normalize 32-bit to float
        fft_input[i * 2 + 1] = 0; // Imaginary part
    }

    // 2. Perform FFT
    dsps_fft2r_fc32(fft_input, n_fft);
    dsps_bit_rev_fc32(fft_input, n_fft);

    // 3. Calculate Power (using first half of complex spectrum)
    mechanical_energy = 0;
    biological_energy = 0;

    // Frequencies (assuming 44.1kHz sample rate):
    // Bin k is at k * (44100 / n_fft) Hz
    float bin_hz = 44100.0f / (float)n_fft;

    for (int i = 0; i < n_fft / 2; i++) {
        float re = fft_input[i * 2 + 0];
        float im = fft_input[i * 2 + 1];
        float power = sqrtf(re * re + im * im);
        float freq = i * bin_hz;

        if (freq >= 100.0f && freq <= 2000.0f) {
            mechanical_energy += power;
        } else if (freq > 2000.0f && freq <= 20000.0f) {
            biological_energy += power;
        }
    }
}
