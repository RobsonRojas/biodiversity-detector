#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>
#include <map>
#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_timer.h" // For uptime

#include "audio_capture.hpp"
#include "audio_buffer.hpp"
#include "dsp_analyzer.hpp"
#include "inference_engine.hpp"
#include "esp_lora_driver.hpp"
#include "battery_monitor.hpp"
#include "telemetry_manager.hpp"

static const char *TAG = "GUARDIAN_MAIN";

// Configuration
#define SAMPLE_RATE         44100
#define FFT_SIZE            1024
#define RMS_THRESHOLD       0.01f  
#define AI_WINDOW_SIZE      (SAMPLE_RATE * 2) 
#define TELEMETRY_INTERVAL  (60 * 1000)      
#define BATTERY_ADC_CHANNEL ADC_CHANNEL_0    // GPIO 1 on S3

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Guardian ESP32-S3 C++ Firmware Starting...");
    
    // Components
    AudioCapture audio;
    AudioBuffer ai_buffer(AI_WINDOW_SIZE);
    DspAnalyzer dsp;
    InferenceEngine ai;
    EspLoRaDriver lora;
    BatteryMonitor battery;
    TelemetryManager telemetry_mgr;

    // Initialization
    if (audio.init(SAMPLE_RATE) != ESP_OK || 
        dsp.init(FFT_SIZE) != ESP_OK || 
        ai.init() != ESP_OK ||
        lora.init() != ESP_OK ||
        battery.init(BATTERY_ADC_CHANNEL) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize components. System Halted.");
        return;
    }

    auto float_samples = std::make_unique<float[]>(FFT_SIZE);
    TickType_t last_telemetry_tick = xTaskGetTickCount();
    
    // Structure to track session stats
    std::map<std::string, int> session_detections;

    ESP_LOGI(TAG, "System Ready. Monitoring Environment...");

    while (1) {
        // --- STAGE 1: RMS Monitoring (Zero-Copy) ---
        const int32_t* raw_samples = (const int32_t*)audio.get_next_buffer();
        const SegmentMetadata* meta = audio.get_next_metadata();
        
        if (raw_samples == NULL || meta == NULL) {
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        // Latency Profiling (Task 3.3)
        uint64_t now = esp_timer_get_time();
        uint32_t propagation_latency_us = (uint32_t)(now - meta->timestamp_us);
        
        static uint32_t sample_count = 0;
        if (sample_count++ % 100 == 0) {
            ESP_LOGI(TAG, "Latency: %lu us | Occupancy: %d/%d", 
                     propagation_latency_us, audio.get_occupancy(), RING_BUFFER_MAX_SEGMENTS);
        }
        
        float rms = 0;
        for (int i = 0; i < FFT_SIZE; i++) {
            float_samples[i] = (float)raw_samples[i] / 2147483648.0f;
            rms += float_samples[i] * float_samples[i];
        }
        rms = sqrtf(rms / FFT_SIZE);

        ai_buffer.push(float_samples.get(), FFT_SIZE);

        if (rms > RMS_THRESHOLD) {
            // --- STAGE 2: DSP Pattern Verification ---
            float mechanical_energy = 0;
            float biological_energy = 0;
            dsp.analyze(raw_samples, mechanical_energy, biological_energy);

            bool should_run_ai = (mechanical_energy > 0.5f) || (biological_energy > 0.2f);

            if (should_run_ai && ai_buffer.is_ready()) {
                // --- STAGE 3: AI Inference ---
                std::string label;
                float confidence;
                
                ai.classify(ai_buffer.get_window(), ai_buffer.get_capacity(), label, confidence);

                if (confidence > 0.8f) {
                    ESP_LOGI(TAG, ">>> DETECTED: %s (Conf: %.2f)", label.c_str(), confidence);
                    
                    // Update session stats
                    session_detections[label]++;
                    
                    // Alert Message
                    std::string payload = "ALERT:" + label + ":" + std::to_string(confidence);
                    lora.send((uint8_t*)payload.c_str(), payload.length());
                }
            }
        }

        // --- Periodic Telemetry ---
        if ((xTaskGetTickCount() - last_telemetry_tick) > pdMS_TO_TICKS(TELEMETRY_INTERVAL)) {
            ESP_LOGI(TAG, "Aggregating real telemetry data...");
            
            TelemetryData data;
            data.battery_level = battery.get_battery_level();
            data.last_rssi = lora.get_last_rssi();
            data.uptime_s = esp_timer_get_time() / 1000000; // microseconds to seconds
            data.detection_counts = session_detections;

            std::string payload = telemetry_mgr.format_payload(data);
            ESP_LOGI(TAG, "Sending telemetry: %s", payload.c_str());
            
            lora.send((uint8_t*)payload.c_str(), payload.length());
            
            last_telemetry_tick = xTaskGetTickCount();
        }

        audio.release_buffer();
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}
