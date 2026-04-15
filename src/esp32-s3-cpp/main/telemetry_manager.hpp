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
#include <map>
#include <stdint.h>
#include <vector>

#define PACKET_TYPE_BEACON 0x01
#define PACKET_TYPE_TELEMETRY 0x02

/**
 * @struct BeaconPacket
 * @brief Binary structure for mesh-wide localization beacons (14 bytes)
 */
struct __attribute__((packed)) BeaconPacket {
    uint8_t type;       // 0x01
    uint8_t node_id;
    uint32_t lat_e7;    // (lat + 90) * 1e7
    uint32_t lon_e7;    // (lon + 180) * 1e7
    uint16_t accuracy;  // meters * 10
    uint8_t hop_count;
};

/**
 * @struct TelemetryPacket
 * @brief Binary structure for periodic telemetry (variable)
 */
struct __attribute__((packed)) TelemetryPacket {
    uint8_t type;       // 0x02
    uint8_t node_id;
    uint8_t battery;    // 0-100
    int8_t rssi;        // dBm
    uint32_t uptime;    // seconds
    uint32_t lat_e7;
    uint32_t lon_e7;
};

/**
 * @brief Structure to hold all telemetry data points.
 */
struct TelemetryData {
    int battery_level;      // 0-100%
    int last_rssi;          // dBm
    uint64_t uptime_s;      // seconds
    double lat;             // degrees
    double lon;             // degrees
    double accuracy;        // meters
    std::map<std::string, int> detection_counts;
};

/**
 * @brief Class to manage and format telemetry strings.
 */
class TelemetryManager {
public:
    TelemetryManager();
    
    /**
     * @brief Increment detection count for a specific label
     * @param label e.g., "Chainsaw", "Insect", "Bird"
     */
    void record_detection(const std::string& label);

    /**
     * @brief Generate a formatted telemetry string
     * @param data Current sensor and system data
     * @return std::string Formatted payload
     */
    std::string format_payload(const TelemetryData& data);

    /**
     * @brief Generate a binary beacon for localization propagation
     */
    void pack_beacon(uint8_t node_id, double lat, double lon, double acc, uint8_t hops, std::vector<uint8_t>& buffer);

    /**
     * @brief Parse a binary beacon
     */
    bool unpack_beacon(const uint8_t* buffer, size_t len, uint8_t& node_id, double& lat, double& lon, double& acc, uint8_t& hops);

private:
    std::map<std::string, int> biodiversity_stats_;
};
