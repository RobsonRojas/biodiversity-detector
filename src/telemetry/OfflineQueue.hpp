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
                    current_msg += line + "\n";
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
