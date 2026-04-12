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

#include "../utils/compat.hpp"
#include "TelegramClient.hpp"
#include <iostream>
#include <vector>
#include <cstdint>

namespace guardian::telemetry {

TelegramClient::TelegramClient(TelegramConfig config) : config_(std::move(config)) {
    // Simulated Initialization
    std::cout << "[TelegramClient] Initialized for chat_id: " << config_.chat_id << std::endl;
}

guardian::expected<void, TelemetryError> TelegramClient::send_message(const std::string& message) {
    // Mock send_message to avoid libcurl cross-compilation issues in Docker
    std::cout << "[TelegramClient] Mock Sending Message to Telegram: " << message << std::endl;
    return {};
}

guardian::expected<void, TelemetryError> TelegramClient::send_audio(const std::string& caption, const std::vector<uint8_t>& data) {
    std::cout << "[TelegramClient] Mock Sending Audio to Telegram. Caption: " << caption 
              << " Data Size: " << data.size() << " bytes" << std::endl;
    return {};
}

} // namespace guardian::telemetry
