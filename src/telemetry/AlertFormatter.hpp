#pragma once

#include <string>
#include "../utils/compat.hpp"
#include <chrono>

namespace guardian::telemetry {

struct DetectionEvent {
    std::string device_id;
    std::string label;
    float confidence;
    std::chrono::system_clock::time_point timestamp;
};

class AlertFormatter {
public:
    static std::string format_detection(const DetectionEvent& event) {
        return guardian::format(
            "ALERT:{} node:{} conf:{:.2f}",
            event.label,
            event.device_id,
            event.confidence
        );
    }
};

} // namespace guardian::telemetry
