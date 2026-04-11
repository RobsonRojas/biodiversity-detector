#include <stdio.h>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

#include "audio_capture.hpp"
#include "dsp_analyzer.hpp"
#include "inference_engine.hpp"

static const char *TAG = "GUARDIAN_MAIN";

// Configuration
#define SAMPLE_RATE     44100
#define FFT_SIZE        1024
#define RMS_THRESHOLD   0.01f  // Trigger for DSP phase

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Guardian ESP32-S3 C++ Firmware Starting...");
    
    // Components
    AudioCapture audio;
    DspAnalyzer dsp;
    InferenceEngine ai;

    // Initialization
    if (audio.init(SAMPLE_RATE) != ESP_OK || 
        dsp.init(FFT_SIZE) != ESP_OK || 
        ai.init() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize components. System Halted.");
        return;
    }

    // Audio buffers
    int32_t* raw_samples = (int32_t*)malloc(FFT_SIZE * sizeof(int32_t));
    size_t bytes_read = 0;

    ESP_LOGI(TAG, "System Ready. Monitoring Environment...");

    while (1) {
        // --- STAGE 1: RMS Monitoring (Simulated Power Conservation) ---
        audio.read(raw_samples, FFT_SIZE * sizeof(int32_t), &bytes_read, portMAX_DELAY);
        
        // Simple RMS calculation
        float rms = 0;
        for (int i = 0; i < FFT_SIZE; i++) {
            float s = (float)raw_samples[i] / 2147483648.0f;
            rms += s * s;
        }
        rms = sqrtf(rms / FFT_SIZE);

        if (rms > RMS_THRESHOLD) {
            ESP_LOGD(TAG, "Threshold triggered (RMS: %.4f). Entering DSP Stage...", rms);

            // --- STAGE 2: DSP Pattern Verification (FFT) ---
            float mechanical_energy = 0;
            float biological_energy = 0;
            dsp.analyze(raw_samples, mechanical_energy, biological_energy);

            ESP_LOGD(TAG, "Energy - Mech: %.4f, Bio: %.4f", mechanical_energy, biological_energy);

            bool should_run_ai = (mechanical_energy > 0.5f) || (biological_energy > 0.2f);

            if (should_run_ai) {
                // --- STAGE 3: AI Inference (TFLite Micro) ---
                std::string label;
                float confidence;
                
                // Note: In real setup, we might need a longer buffer for the CNN
                // For now we use the normalized data from Stage 2 logic
                float* normalized_buffer = (float*)malloc(FFT_SIZE * sizeof(float));
                for(int i=0; i<FFT_SIZE; i++) normalized_buffer[i] = (float)raw_samples[i] / 2147483648.0f;
                
                ai.classify(normalized_buffer, FFT_SIZE, label, confidence);
                free(normalized_buffer);

                if (confidence > 0.8f) {
                    ESP_LOGI(TAG, ">>> DETECTED: %s (Conf: %.2f)", label.c_str(), confidence);
                    // TODO: Trigger LoRa Alert / Telemetry
                }
            }
        }

        // Heartbeat / Power Management
        vTaskDelay(pdMS_TO_TICKS(10)); // Minimal yield
    }
}
