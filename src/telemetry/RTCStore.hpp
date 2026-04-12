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
