#include <iostream>
#include <thread>
#include <chrono>
#include "audio/AudioIn.hpp"
#include "audio/CircularBuffer.hpp"
#include "ai/DetectionEngine.hpp"
#include "AlertManager.hpp"
#include <atomic>

using namespace guardian;

std::atomic<bool> running{true};

void detection_loop(audio::AudioIn& audio_in, audio::CircularBuffer& buffer, ai::DetectionEngine& engine) {
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
                    // AlertManager would push this to Telegram/LoRa
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // 2. SLEEP PHASE (55 seconds)
        std::cout << "[SYSTEM] Entering 55s Deep Sleep Phase..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(55));
    }
}

int main() {
    audio::AudioIn audio_in;
    audio::CircularBuffer buffer(32000); // 2 seconds capacity
    ai::DetectionEngine engine;

    if (!audio_in.open()) {
        std::cerr << "Failed to open audio device." << std::endl;
        return 1;
    }

    if (!engine.initialize("assets/models/motoserra_detect_v1.tflite")) {
        std::cerr << "Failed to initialize AI engine." << std::endl;
        return 1;
    }

    detection_loop(audio_in, buffer, engine);

    return 0;
}
