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

#include "../utils/compat.hpp"
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
