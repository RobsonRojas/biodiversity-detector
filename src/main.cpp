#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <memory>

#include "utils/compat.hpp"
#include "audio/CascadeFilter.hpp"
#include "ai/EdgeImpulseWrapper.hpp"
#include "hal/esp32/ESP32Power.hpp"
#include "telemetry/RTCStore.hpp"
#include "audio/AudioIn.hpp"
#include "audio/CircularBuffer.hpp"
#include "ai/DetectionEngine.hpp"
#include "AlertManager.hpp"
#include "telemetry/ConfigParser.hpp"
#include "telemetry/LoRaDriver.hpp"
#include "telemetry/SupabaseClient.hpp"
#include "telemetry/TelegramClient.hpp"
#include "telemetry/OfflineQueue.hpp"
#include "telemetry/localization_module.hpp"

using namespace guardian;

std::atomic<bool> running{true};

void heartbeat_loop(AlertManager& alert_manager) {
    while (running) {
        // Send heartbeat immediately then sleep
        uint16_t mock_bat = 3800 + (rand() % 400); 
        int8_t mock_rssi = -70 - (rand() % 40);
        
        std::cout << "[SYSTEM] Sending periodic Mesh Heartbeat (Bat: " << mock_bat << "mV, RSSI: " << (int)mock_rssi << "dBm)..." << std::endl;
        
        // Broadcast location if we have it
        NodeCoords pos = alert_manager.get_localization_engine()->get_current_coords();
        if (pos.accuracy < 500) {
            std::string beacon = "BEACON:" + std::to_string(alert_manager.get_config().node_id) +
                                ":" + std::to_string(pos.lat) +
                                ":" + std::to_string(pos.lon) +
                                ":" + std::to_string(pos.accuracy) +
                                ":" + std::to_string(pos.hop_count);
            // We piggyback location on heartbeats for simulation simplicity
            alert_manager.send_heartbeat(mock_bat, mock_rssi, beacon);
        } else {
            alert_manager.send_heartbeat(mock_bat, mock_rssi);
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}

void mesh_receive_loop(std::shared_ptr<telemetry::LoRaDriver> lora, AlertManager& alert_manager) {
    uint8_t rx_buf[1024];
    while (running) {
        if (lora != nullptr) {
            auto rx_res = lora->receive(guardian::span<uint8_t>(rx_buf, sizeof(rx_buf)));
            if (rx_res && *rx_res > 0) {
                alert_manager.on_mesh_receive(guardian::span<const uint8_t>(rx_buf, *rx_res));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void detection_loop(const telemetry::NodeConfig& config, audio::AudioIn& audio_in, audio::CircularBuffer& buffer, ai::DetectionEngine& engine, AlertManager& alert_manager, std::shared_ptr<telemetry::LoRaDriver> lora) {
    audio::CascadeFilter cascade(0.01f); // 1% RMS threshold
    ai::EdgeImpulseWrapper ei_classifier;
    std::vector<int16_t> capture_buffer(4096);

    while (running) {
        // --- STAGE 1 & 2: MOCK BYPASS FOR SIMULATION ---
        const char* node_id_env = std::getenv("NODE_ID");
        std::string node_id_str = node_id_env ? node_id_env : "unknown";
        std::string chainsaw_trig_path = "/tmp/" + node_id_str + "_chainsaw_trigger";
        std::string frog_trig_path = "/tmp/" + node_id_str + "_frog_trigger";
        bool trigger_active = (::access(chainsaw_trig_path.c_str(), F_OK) == 0) || (::access(frog_trig_path.c_str(), F_OK) == 0);
        
        auto result = audio_in.read_int16(capture_buffer); // Read low-rate for RMS
        if (trigger_active || (result && cascade.check_rms_threshold(capture_buffer))) {
            if (trigger_active) {
                std::cout << "[SIM] Trigger detected! Bypassing Stage 1 & 2..." << std::endl;
            } else {
                std::cout << "[POWER] Stage 1 Triggered! Waking for DSP Verification..." << std::endl;
            }

            // --- STAGE 2: DSP VERIFICATION ---
            if (trigger_active || cascade.verify_frequency_pattern(capture_buffer)) {
                if (!trigger_active) {
                    std::cout << "[POWER] Stage 2 Pattern Matched! Starting AI Inference..." << std::endl;
                }

                // --- STAGE 3: AI INFERENCE ---
                auto classifications = ei_classifier.classify(capture_buffer);
                
                for (const auto& res : classifications) {
                    if (res.label != "Background" && res.confidence > 0.80f) {
                        std::cout << "[ALERT] " << res.label << " detected! Confidence: " << res.confidence << std::endl;
                        
                        // Log to RTC for persistence
                        telemetry::RTCStore::log_event(res.label, res.confidence);

                        // Telemetry
                        telemetry::DetectionEvent event;
                        event.label = res.label;
                        event.confidence = res.confidence;
                        event.device_id = std::to_string(config.node_id);
                        alert_manager.on_detection(event);
                    }
                }
            } else {
                std::cout << "[POWER] Stage 2 Rejected. Returning to Stage 1 Sleep..." << std::endl;
            }
        }

        // Handle sleep behavior
        const char* disable_sleep = std::getenv("SIM_DISABLE_SLEEP");
        if (disable_sleep && std::string(disable_sleep) == "1") {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } else {
            hal::ESP32Power::enter_deep_sleep();
        }
    }
}

int main(int argc, char* argv[]) {
    std::string device_path = "/dev/forest_audio";
    if (argc > 1) {
        device_path = argv[1];
        std::cout << "[INFO] Using custom device path: " << device_path << std::endl;
    }

    auto config = telemetry::ConfigParser::parse_from_env();
    auto lora = config.driver;
    
    if (!lora) {
        std::cerr << "[ERROR] No LoRa driver configured!" << std::endl;
        return 1;
    }

    if (auto init_res = lora->initialize(); !init_res) {
        std::cerr << "[ERROR] Failed to initialize LoRa driver: " << init_res.error().message() << std::endl;
        // Don't return 1 here if we want to support partial starts, but for now let's be strict
        return 1;
    }
    
    // Client and queue could be properly initialized if needed
    auto client = std::make_shared<telemetry::TelegramClient>(telemetry::TelegramConfig{"dummy", "dummy"});
    auto queue = std::make_shared<telemetry::OfflineQueue>("queue.db");
    auto supabase = std::make_shared<telemetry::SupabaseClient>("https://dummy.supabase.co", "dummy_key");

    LocalizationEngine localization;
    const char* role_env = std::getenv("NODE_ROLE");
    if (role_env && std::string(role_env) == "Gateway") {
        const char* lat_env = std::getenv("SIM_LAT");
        const char* lon_env = std::getenv("SIM_LON");
        if (lat_env && lon_env) {
            localization.set_anchor(std::stod(lat_env), std::stod(lon_env));
            std::cout << "[Localization] Gateway Anchor Set: " << lat_env << ", " << lon_env << std::endl;
        }
    }

    AlertManager alert_manager(config, client, queue, lora, supabase, &localization);

    audio::AudioIn audio_in(device_path);
    audio::CircularBuffer buffer(32000); // 2 seconds capacity
    ai::DetectionEngine engine;

    if (!audio_in.open()) {
        std::cerr << "Failed to open audio device: " << device_path << std::endl;
        const char* lora_driver = std::getenv("LORA_DRIVER");
        if (lora_driver && std::string(lora_driver) == "physical") {
            return 1;
        }
        std::cout << "[SIM] Continuing without physical audio device..." << std::endl;
    }

    if (!engine.initialize("assets/models/motoserra_detect_v1.tflite")) {
        std::cerr << "Failed to initialize AI engine." << std::endl;
        return 1;
    }

    std::thread rx_thread(mesh_receive_loop, lora, std::ref(alert_manager));
    std::thread hb_thread(heartbeat_loop, std::ref(alert_manager));
    detection_loop(config, audio_in, buffer, engine, alert_manager, lora);

    rx_thread.join();
    hb_thread.join();

    return 0;
}
