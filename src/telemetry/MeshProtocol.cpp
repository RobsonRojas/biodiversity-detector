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
#include "MeshProtocol.hpp"
#include <cstring>

namespace guardian::telemetry {

std::vector<uint8_t> MeshPacket::serialize() const {
    std::vector<uint8_t> result;
    result.resize(sizeof(MeshHeader) + payload.size());
    std::memcpy(result.data(), &header, sizeof(MeshHeader));
    if (!payload.empty()) {
        std::memcpy(result.data() + sizeof(MeshHeader), payload.data(), payload.size());
    }
    return result;
}

std::optional<MeshPacket> MeshPacket::deserialize(guardian::span<const uint8_t> data) {
    if (data.size() < sizeof(MeshHeader)) return std::nullopt;
    MeshPacket pkt;
    std::memcpy(&pkt.header, data.data(), sizeof(MeshHeader));
    if (data.size() >= sizeof(MeshHeader) + pkt.header.payload_len) {
        pkt.payload.assign(data.data() + sizeof(MeshHeader), 
                           data.data() + sizeof(MeshHeader) + pkt.header.payload_len);
    }
    return pkt;
}

RouteManager::RouteManager(uint16_t local_id) : local_id_(local_id) {}

void RouteManager::add_route(uint16_t destination, uint16_t next_hop) {
    routing_table_[destination] = next_hop;
}

std::optional<uint16_t> RouteManager::get_next_hop(uint16_t destination) const {
    auto it = routing_table_.find(destination);
    if (it != routing_table_.end()) {
        return it->second;
    }
    return std::nullopt;
}

} // namespace guardian::telemetry
