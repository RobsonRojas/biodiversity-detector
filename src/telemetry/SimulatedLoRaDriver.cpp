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

#include "SimulatedLoRaDriver.hpp"
#include "../utils/compat.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <netdb.h>

namespace guardian::telemetry {

SimulatedLoRaDriver::SimulatedLoRaDriver(int port, const std::vector<std::string>& neighbors) 
    : sock_fd_(-1), port_(port), neighbors_(neighbors) {}

SimulatedLoRaDriver::~SimulatedLoRaDriver() {
    if (sock_fd_ != -1) {
        close(sock_fd_);
    }
}

guardian::expected<void, std::error_code> SimulatedLoRaDriver::initialize() {
    if ((sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return guardian::unexpected(std::make_error_code(std::errc::io_error));
    }

    memset(&servaddr_, 0, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_addr.s_addr = INADDR_ANY;
    servaddr_.sin_port = htons(port_);

    if (bind(sock_fd_, (const struct sockaddr *)&servaddr_, sizeof(servaddr_)) < 0) {
        return guardian::unexpected(std::make_error_code(std::errc::address_in_use));
    }

    std::cout << "[SimulatedLoRa] Initialized on port " << port_ << " with " << neighbors_.size() << " neighbors.
";
    return {};
}

guardian::expected<void, std::error_code> SimulatedLoRaDriver::send(guardian::span<const uint8_t> data) {
    if (sock_fd_ == -1) return guardian::unexpected(std::make_error_code(std::errc::not_connected));

    for (const auto& neighbor : neighbors_) {
        std::string host = neighbor;
        int port = port_; // Default to local port if not specified
        
        size_t colon = neighbor.find(':');
        if (colon != std::string::npos) {
            host = neighbor.substr(0, colon);
            port = std::stoi(neighbor.substr(colon + 1));
        }

        struct sockaddr_in dest;
        memset(&dest, 0, sizeof(dest));
        dest.sin_family = AF_INET;
        dest.sin_port = htons(port);
        
        struct hostent *he = gethostbyname(host.c_str());
        if (he) {
            memcpy(&dest.sin_addr, he->h_addr_list[0], he->h_length);
            ssize_t sent = sendto(sock_fd_, data.data(), data.size(), 0, (const struct sockaddr *)&dest, sizeof(dest));
            if (sent >= 0) {
                std::cout << "[SimulatedLoRa] SENT " << sent << " bytes to " << host << ":" << port << std::endl;
            } else {
                std::cerr << "[SimulatedLoRa] Failed to send to " << neighbor << ": " << strerror(errno) << std::endl;
            }
        } else {
            std::cerr << "[SimulatedLoRa] Could not resolve neighbor host: " << host << "
";
        }
    }
    return {};
}

guardian::expected<size_t, std::error_code> SimulatedLoRaDriver::receive(guardian::span<uint8_t> buffer) {
    if (sock_fd_ == -1) return guardian::unexpected(std::make_error_code(std::errc::not_connected));

    struct sockaddr_in src;
    socklen_t len = sizeof(src);
    ssize_t n = recvfrom(sock_fd_, buffer.data(), buffer.size_bytes(), 0, (struct sockaddr *)&src, &len);
    
    if (n >= 0) {
        char sender_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &src.sin_addr, sender_ip, INET_ADDRSTRLEN);
        std::cout << "[SimulatedLoRa] RECV " << n << " bytes from " << sender_ip << std::endl;
    } else {
        return guardian::unexpected(std::make_error_code(std::errc::io_error));
    }

    return static_cast<size_t>(n);
}

} // namespace guardian::telemetry
