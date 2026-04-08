#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "../utils/compat.hpp"

namespace guardian::telemetry {

enum class TelemetryError {
    NetworkError,
    ApiError,
    InvalidToken,
    Timeout,
    Unknown
};

struct TelegramConfig {
    std::string bot_token;
    std::string chat_id;
};

class TelegramClient {
public:
    explicit TelegramClient(TelegramConfig config);
    
    /**
     * @brief Sends a message to the configured Telegram chat.
     * @param message HTML formatted message string.
     * @return guardian::expected<void, TelemetryError>
     */
    guardian::expected<void, TelemetryError> send_message(const std::string& message);
    guardian::expected<void, TelemetryError> send_audio(const std::string& caption, const std::vector<uint8_t>& data);

private:
    TelegramConfig config_;
};

} // namespace guardian::telemetry
