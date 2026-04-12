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
