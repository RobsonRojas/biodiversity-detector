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
#include "LoRaDriver.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

namespace guardian::telemetry {

LoRaDriver::LoRaDriver() : fd_(-1) {}

LoRaDriver::~LoRaDriver() {
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

guardian::expected<void, std::error_code> LoRaDriver::initialize() {
    // Simulated AU915 initialization
    fd_ = 1; // Fake valid fd for simulation
    std::cout << "[LoRaDriver] Initialized LoRaWAN AU915 mode." << std::endl;
    return {};
}

guardian::expected<void, std::error_code> LoRaDriver::send(guardian::span<const uint8_t> data) {
    if (fd_ < 0) return guardian::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    std::cout << "[LoRaDriver] Sending " << data.size() << " bytes via LoRa..." << std::endl;
    // Simulate sending
    return {};
}

guardian::expected<size_t, std::error_code> LoRaDriver::receive(guardian::span<uint8_t> buffer) {
    if (fd_ < 0) return guardian::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    // Simulate no immediate incoming data
    return 0; 
}

} // namespace guardian::telemetry
