#include "RTCStore.hpp"
#include <cstring>

namespace guardian::telemetry {

// The actual RTC memory location
static RTC_DATA_ATTR DetectionLog s_last_event;

void RTCStore::log_event(const std::string& label, float confidence) {
    std::strncpy(s_last_event.label, label.c_str(), sizeof(s_last_event.label) - 1);
    s_last_event.confidence = confidence;
    // timestamp_now would be populated here
}

void RTCStore::get_last_event(DetectionLog& log) {
    std::memcpy(&log, &s_last_event, sizeof(DetectionLog));
}

} // namespace guardian::telemetry
