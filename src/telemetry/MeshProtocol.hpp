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
#include <vector>
#include <map>
#include <string>
#include <optional>
#include "../utils/compat.hpp"

#if __has_include(<flat_map>)
#include <flat_map>
#endif

namespace guardian::telemetry {

#pragma pack(push, 1)
struct MeshHeader {
    uint16_t target_id;
    uint16_t sender_id;
    uint16_t prev_hop_id;
    uint16_t battery_mv; // Diagnostic: battery voltage
    int8_t last_rssi;    // Diagnostic: RSSI of previous hop
    uint8_t hop_limit;
    uint8_t payload_len;
    double lat;          // Localization: Latitude
    double lon;          // Localization: Longitude
    uint16_t signature;  // simplified signature for demo
};
#pragma pack(pop)

class MeshPacket {
public:
    MeshHeader header;
    std::vector<uint8_t> payload;

    std::vector<uint8_t> serialize() const;
    static std::optional<MeshPacket> deserialize(guardian::span<const uint8_t> data);

    bool verify_signature() const {
        // Simplified signature verification
        return header.signature == 0xABCD;
    }
};

class RouteManager {
public:
    RouteManager(uint16_t local_id);
    void add_route(uint16_t destination, uint16_t next_hop);
    std::optional<uint16_t> get_next_hop(uint16_t destination) const;
private:
    uint16_t local_id_;
#if __has_include(<flat_map>)
    std::flat_map<uint16_t, uint16_t> routing_table_;
#else
    std::map<uint16_t, uint16_t> routing_table_;
#endif
};

} // namespace guardian::telemetry
