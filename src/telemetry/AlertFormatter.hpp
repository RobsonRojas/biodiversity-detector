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
