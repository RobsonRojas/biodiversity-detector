#include "utils/compat.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include "audio/AudioIn.hpp"
#include "audio/CircularBuffer.hpp"
#include "ai/DetectionEngine.hpp"
#include "AlertManager.hpp"
#include "telemetry/ConfigParser.hpp"
#include "telemetry/LoRaDriver.hpp"
#include "telemetry/SimulatedLoRaDriver.hpp"
#include "telemetry/SupabaseClient.hpp"
#include <atomic>
#include <cstdlib>
#include <unistd.h>
#include <thread>

using namespace guardian;

std::atomic<bool> running{true};

void heartbeat_loop(AlertManager& alert_manager) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        std::cout << "[SYSTEM] Sending periodic Mesh Heartbeat..." << std::endl;
        alert_manager.send_heartbeat();
    }
}

void mesh_receive_loop(std::shared_ptr<telemetry::LoRaDriver> lora, AlertManager& alert_manager) {
    uint8_t rx_buf[1024];
    while (running) {
        if (lora != nullptr) {
            auto rx_res = lora->receive(rx_buf);
            if (rx_res && *rx_res > 0) {
                alert_manager.on_mesh_receive(std::span<const uint8_t>(rx_buf, *rx_res));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void detection_loop(const telemetry::NodeConfig& config, audio::AudioIn& audio_in, audio::CircularBuffer& buffer, ai::DetectionEngine& engine, AlertManager& alert_manager, std::shared_ptr<telemetry::LoRaDriver> lora) {
    std::vector<int32_t> capture_buffer(4096);
    std::vector<int32_t> inference_frame(16000);

    while (running) {
        // 1. ACTIVE PHASE (5 seconds)
        auto start = std::chrono::steady_clock::now();
        std::cout << "[SYSTEM] Starting 5s Active Phase..." << std::endl;

        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) {
            auto result = audio_in.read(capture_buffer);
            if (result) {
                buffer.push(std::span(capture_buffer.data(), *result));
            }

            if (buffer.size() >= 16000) {
                buffer.read_latest(inference_frame);
                auto confidence = engine.detect_motoserra(inference_frame);
                
                // Mock Trigger via Filesystem (For Simulation)
                const char* trigger = std::getenv("SIM_DETECTION_TRIGGER");
                if (trigger) {
                    if (access(trigger, F_OK) == 0) {
                        std::cout << "[SIM] Forces Detection Trigger Found!" << std::endl;
                        confidence = 0.99f;
                    }
                }

                if (confidence && *confidence > 0.85f) {
                    std::cout << "[ALERT] Motoserra detected! Confidence: " << *confidence << std::endl;
                    telemetry::DetectionEvent event;
                    event.confidence = *confidence;
                    event.device_id = std::to_string(config.node_id);
                    alert_manager.on_detection(event);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // 2. SLEEP PHASE (55 seconds)
        std::cout << "[SYSTEM] Entering 55s Deep Sleep Phase..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(55));
    }
}

int main(int argc, char* argv[]) {
    std::string device_path = "/dev/forest_audio";
    if (argc > 1) {
        device_path = argv[1];
        std::cout << "[INFO] Using custom device path: " << device_path << std::endl;
    }

    auto config = telemetry::ConfigParser::parse_static_config("config.yaml");
    
    std::shared_ptr<telemetry::LoRaDriver> lora;
    const char* use_sim = std::getenv("USE_SIM_LORA");
    if (use_sim && std::string(use_sim) == "1") {
        std::cout << "[INFO] Using Simulated LoRa Driver (UDP)" << std::endl;
        lora = std::make_shared<telemetry::SimulatedLoRaDriver>();
    } else {
        std::cout << "[INFO] Using Physical LoRa Driver (SX1262)" << std::endl;
        lora = std::make_shared<telemetry::LoRaDriver>();
    }
    
    lora->initialize();
    
    // Client and queue could be properly initialized if needed
    auto client = std::make_shared<telemetry::TelegramClient>(telemetry::TelegramConfig{"dummy", "dummy"});
    auto queue = std::make_shared<telemetry::OfflineQueue>("queue.db");

    const char* sb_url = std::getenv("SUPABASE_URL");
    const char* sb_key = std::getenv("SUPABASE_KEY");
    auto supabase = std::make_shared<telemetry::SupabaseClient>(
        sb_url ? sb_url : "https://dummy.supabase.co", 
        sb_key ? sb_key : "dummy_key"
    );
    
    AlertManager alert_manager(config, client, queue, lora, supabase);

    audio::AudioIn audio_in(device_path);
    audio::CircularBuffer buffer(32000); // 2 seconds capacity
    ai::DetectionEngine engine;

    if (!audio_in.open()) {
        std::cerr << "Failed to open audio device: " << device_path << std::endl;
        if (!use_sim || std::string(use_sim) != "1") {
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
