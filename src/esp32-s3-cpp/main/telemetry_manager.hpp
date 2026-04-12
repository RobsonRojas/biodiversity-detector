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
