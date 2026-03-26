#include "TelegramClient.hpp"
#include <iostream>

namespace guardian::telemetry {

TelegramClient::TelegramClient(TelegramConfig config) : config_(std::move(config)) {
    // Simulated Initialization
    std::cout << "[TelegramClient] Initialized for chat_id: " << config_.chat_id << std::endl;
}

std::expected<void, TelemetryError> TelegramClient::send_message(const std::string& message) {
    // Mock send_message to avoid libcurl cross-compilation issues in Docker
    std::cout << "[TelegramClient] Mock Sending Message to Telegram:\n" << message << std::endl;
    return {};
}

} // namespace guardian::telemetry
