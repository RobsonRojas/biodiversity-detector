#pragma once
#include "../utils/compat.hpp"

#include <cstdint>
#include <string>

namespace guardian::telemetry {

// RTC-mapped storage for persistence across deep sleep cycles
#ifdef ESP_PLATFORM
#define RTC_DATA_ATTR __attribute__((section(".rtc.data")))
#else
#define RTC_DATA_ATTR 
#endif

struct DetectionLog {
    char label[32];
    float confidence;
    uint32_t timestamp;
};

class RTCStore {
public:
    /**
     * @brief Log a detection result to RTC memory.
     */
    static void log_event(const std::string& label, float confidence);

    /**
     * @brief Retrieve last log from RTC memory.
     */
    static void get_last_event(DetectionLog& log);
};

} // namespace guardian::telemetry
