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

private:
    std::map<std::string, int> biodiversity_stats_;
};
