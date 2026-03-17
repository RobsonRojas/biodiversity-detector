#pragma once

#include <string>
#include "../utils/compat.hpp"
#include <chrono>

namespace guardian::telemetry {

struct DetectionEvent {
    std::string device_id;
    float confidence;
    std::chrono::system_clock::time_point timestamp;
};

class AlertFormatter {
public:
    static std::string format_detection(const DetectionEvent& event) {
        auto now = std::chrono::system_clock::to_time_t(event.timestamp);
        std::string time_str = std::ctime(&now);
        if (!time_str.empty()) time_str.pop_back(); // Remove newline

        return std::format(
            "🚨 <b>Motoserra Detectada!</b>\n\n"
            "📍 <b>Dispositivo:</b> {}\n"
            "📊 <b>Confiança:</b> {:.2f}%\n"
            "🕒 <b>Horário:</b> {}\n"
            "⚠️ <i>Ação imediata recomendada.</i>",
            event.device_id,
            event.confidence * 100.0f,
            time_str
        );
    }
};

} // namespace guardian::telemetry
