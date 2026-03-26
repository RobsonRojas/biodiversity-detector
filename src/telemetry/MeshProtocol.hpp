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
    uint16_t signature; // simplified signature for demo
};
#pragma pack(pop)

class MeshPacket {
public:
    MeshHeader header;
    std::vector<uint8_t> payload;

    std::vector<uint8_t> serialize() const;
    static std::optional<MeshPacket> deserialize(std::span<const uint8_t> data);

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
