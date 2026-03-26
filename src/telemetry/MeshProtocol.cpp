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

std::optional<MeshPacket> MeshPacket::deserialize(std::span<const uint8_t> data) {
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
