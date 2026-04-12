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

#include "LoRaDriver.hpp"
#include "../utils/compat.hpp"
#include <netinet/in.h>
#include <string>
#include <vector>

namespace guardian::telemetry {

class SimulatedLoRaDriver : public LoRaDriver {
public:
    SimulatedLoRaDriver(int port, const std::vector<std::string>& neighbors);
    ~SimulatedLoRaDriver() override;

    guardian::expected<void, std::error_code> initialize() override;
    guardian::expected<void, std::error_code> send(guardian::span<const uint8_t> data) override;
    guardian::expected<size_t, std::error_code> receive(guardian::span<uint8_t> buffer) override;

private:
    int sock_fd_;
    int port_;
    std::vector<std::string> neighbors_;
    struct sockaddr_in servaddr_;
};

} // namespace guardian::telemetry
