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

#include "telemetry_manager.hpp"
#include <sstream>

TelemetryManager::TelemetryManager() {}

void TelemetryManager::record_detection(const std::string& label) {
    biodiversity_stats_[label]++;
}

void TelemetryManager::pack_beacon(uint8_t node_id, double lat, double lon, double acc, uint8_t hops, std::vector<uint8_t>& buffer) {
    BeaconPacket packet;
    packet.type = PACKET_TYPE_BEACON;
    packet.node_id = node_id;
    packet.lat_e7 = (uint32_t)((lat + 90.0) * 1e7);
    packet.lon_e7 = (uint32_t)((lon + 180.0) * 1e7);
    packet.accuracy = (uint16_t)(acc * 10.0);
    packet.hop_count = hops;

    buffer.assign((uint8_t*)&packet, (uint8_t*)&packet + sizeof(BeaconPacket));
}

bool TelemetryManager::unpack_beacon(const uint8_t* buffer, size_t len, uint8_t& node_id, double& lat, double& lon, double& acc, uint8_t& hops) {
    if (len < sizeof(BeaconPacket)) return false;
    
    const BeaconPacket* packet = (const BeaconPacket*)buffer;
    if (packet->type != PACKET_TYPE_BEACON) return false;

    node_id = packet->node_id;
    lat = ((double)packet->lat_e7 / 1e7) - 90.0;
    lon = ((double)packet->lon_e7 / 1e7) - 180.0;
    acc = (double)packet->accuracy / 10.0;
    hops = packet->hop_count;

    return true;
}

std::string TelemetryManager::format_payload(const TelemetryData& data) {
    std::stringstream ss;
    ss << "TELEMETRY"
       << ":BAT:" << data.battery_level
       << ":RSSI:" << data.last_rssi
       << ":UPTIME:" << data.uptime_s
       << ":LAT:" << data.lat
       << ":LON:" << data.lon
       << ":ACC:" << data.accuracy;
    
    // Append biodiversity stats
    for (const auto& [label, count] : data.detection_counts) {
        ss << ":" << label << ":" << count;
    }
    
    return ss.str();
}
