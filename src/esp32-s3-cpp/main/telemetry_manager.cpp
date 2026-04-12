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
