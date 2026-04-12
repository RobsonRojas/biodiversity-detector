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

#include <cstdint>
#include <string>
#include <vector>
#include <system_error>
#include "../utils/compat.hpp"

namespace guardian::telemetry {

class LoRaDriver {
public:
    LoRaDriver();
    virtual ~LoRaDriver();

    virtual guardian::expected<void, std::error_code> initialize();

    // 1.2 Implement a buffer manager using `guardian::span` for zero-copy packet handling.
    virtual guardian::expected<void, std::error_code> send(guardian::span<const uint8_t> data);
    virtual guardian::expected<size_t, std::error_code> receive(guardian::span<uint8_t> buffer);

private:
    int fd_;
};

} // namespace guardian::telemetry
