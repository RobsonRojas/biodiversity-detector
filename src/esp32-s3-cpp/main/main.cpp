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
#include "localization_module.hpp"

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
    LocalizationEngine localization;

    // Initialization
    if (audio.init(SAMPLE_RATE) != ESP_OK || 
        dsp.init(FFT_SIZE) != ESP_OK || 
        ai.init() != ESP_OK ||
        lora.init() != ESP_OK ||
        battery.init(BATTERY_ADC_CHANNEL) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize components. System Halted.");
        return;
    }

    // Role-based configuration for localization
    const char* role_env = getenv("NODE_ROLE");
    if (role_env && strcmp(role_env, "Gateway") == 0) {
        const char* lat_env = getenv("SIM_LAT");
        const char* lon_env = getenv("SIM_LON");
        if (lat_env && lon_env) {
            double lat = strtod(lat_env, NULL);
            double lon = strtod(lon_env, NULL);
            localization.set_anchor(lat, lon);
            ESP_LOGI(TAG, "Node configured as GATEWAY Anchor at (%f, %f)", lat, lon);
        }
    }

    // Link LoRa to Localization for simulation propagation
    lora.set_localization_engine(&localization);

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
            lora.poll(); // Poll LoRa even if no audio buffer is ready
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
                    // Alert Message with Location
                    NodeCoords current_pos = localization.get_current_coords();
                    std::string payload = "ALERT:" + label + ":" + std::to_string(confidence) +
                                         ":LAT:" + std::to_string(current_pos.lat) +
                                         ":LON:" + std::to_string(current_pos.lon);
                    lora.send((uint8_t*)payload.c_str(), payload.length());
                }
            }
        }

        // --- Periodic Telemetry & Beacon ---
        if ((xTaskGetTickCount() - last_telemetry_tick) > pdMS_TO_TICKS(TELEMETRY_INTERVAL)) {
            ESP_LOGI(TAG, "Aggregating telemetry and broadcasting beacon...");
            
            TelemetryData data;
            data.battery_level = battery.get_battery_level();
            data.last_rssi = lora.get_last_rssi();
            data.uptime_s = esp_timer_get_time() / 1000000;
            
            NodeCoords current_pos = localization.get_current_coords();
            data.lat = current_pos.lat;
            data.lon = current_pos.lon;
            data.accuracy = current_pos.accuracy;
            data.detection_counts = session_detections;

            // 1. Send Standard Telemetry (String for legacy/debug)
            std::string payload = telemetry_mgr.format_payload(data);
            lora.send((uint8_t*)payload.c_str(), payload.length());
            
            // 2. Send Localization Beacon (Binary)
            // Only broadcast as an anchor if we have reasonable accuracy
            if (current_pos.accuracy < 500.0) {
                std::vector<uint8_t> beacon_buf;
                telemetry_mgr.pack_beacon(1, current_pos.lat, current_pos.lon, 
                                        current_pos.accuracy, current_pos.hop_count, beacon_buf);
                lora.send(beacon_buf.data(), beacon_buf.size());
                ESP_LOGI(TAG, "Broadcasted binary beacon (Hop: %d, Acc: %.1fm)", 
                         current_pos.hop_count, current_pos.accuracy);
            }
            
            last_telemetry_tick = xTaskGetTickCount();
        }

        audio.release_buffer();
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}
