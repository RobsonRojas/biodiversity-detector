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

#include <string>
#include <vector>
#include <fstream>
#include <mutex>

namespace guardian::telemetry {

class OfflineQueue {
public:
    explicit OfflineQueue(std::string file_path) : file_path_(std::move(file_path)) {}

    void push(const std::string& message) {
        std::lock_guard lock(mutex_);
        std::ofstream file(file_path_, std::ios::app);
        if (file.is_open()) {
            file << message << "||END_MSG||" << std::endl;
        }
    }

    std::vector<std::string> pop_all() {
        std::lock_guard lock(mutex_);
        std::vector<std::string> messages;
        std::ifstream file(file_path_);
        if (file.is_open()) {
            std::string line;
            std::string current_msg;
            while (std::getline(file, line)) {
                size_t pos = line.find("||END_MSG||");
                if (pos != std::string::npos) {
                    current_msg += line.substr(0, pos);
                    messages.push_back(current_msg);
                    current_msg.clear();
                } else {
                    current_msg += line + "
";
                }
            }
        }
        std::ofstream clear_file(file_path_, std::ios::trunc);
        return messages;
    }

private:
    std::string file_path_;
    std::mutex mutex_;
};

} // namespace guardian::telemetry
