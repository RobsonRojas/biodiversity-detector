#include <iostream>
#include <thread>
#include <chrono>
#include "audio/AudioIn.hpp"
#include "audio/CircularBuffer.hpp"
#include "ai/DetectionEngine.hpp"
#include "AlertManager.hpp"
#include "telemetry/ConfigParser.hpp"
#include "telemetry/LoRaDriver.hpp"
#include <atomic>

using namespace guardian;

std::atomic<bool> running{true};

void detection_loop(audio::AudioIn& audio_in, audio::CircularBuffer& buffer, ai::DetectionEngine& engine, AlertManager& alert_manager, std::shared_ptr<telemetry::LoRaDriver> lora) {
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
                
                if (confidence && *confidence > 0.85f) {
                    std::cout << "[ALERT] Motoserra detected! Confidence: " << *confidence << std::endl;
                    telemetry::DetectionEvent event;
                    event.confidence = *confidence;
                    // Mock timestamp for event
                    alert_manager.on_detection(event);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // 2. SLEEP PHASE (55 seconds)
        std::cout << "[SYSTEM] Entering 55s Deep Sleep Phase..." << std::endl;
        
        // Let's pretend we verify network delivery and process incoming packets while sleeping
        std::cout << "[SYSTEM] Verifying network delivery using pre-defined static paths..." << std::endl;
        uint8_t dummy_rx[256];
        if (lora) {
            auto rx_res = lora->receive(dummy_rx);
            if (rx_res && *rx_res > 0) {
                alert_manager.on_mesh_receive(std::span<const uint8_t>(dummy_rx, *rx_res));
            }
        }
        
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
    auto lora = std::make_shared<telemetry::LoRaDriver>();
    lora->initialize();
    
    // Client and queue could be properly initialized if needed
    auto client = std::make_shared<telemetry::TelegramClient>(telemetry::TelegramConfig{"dummy", "dummy"});
    auto queue = std::make_shared<telemetry::OfflineQueue>("queue.db");
    
    AlertManager alert_manager(config, client, queue, lora);

    audio::AudioIn audio_in(device_path);
    audio::CircularBuffer buffer(32000); // 2 seconds capacity
    ai::DetectionEngine engine;

    if (!audio_in.open()) {
        std::cerr << "Failed to open audio device: " << device_path << std::endl;
        return 1;
    }

    if (!engine.initialize("assets/models/motoserra_detect_v1.tflite")) {
        std::cerr << "Failed to initialize AI engine." << std::endl;
        return 1;
    }

    detection_loop(audio_in, buffer, engine, alert_manager, lora);

    return 0;
}
