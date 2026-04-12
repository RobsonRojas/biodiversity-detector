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
