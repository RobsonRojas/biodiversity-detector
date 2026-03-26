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

SimulatedLoRaDriver::SimulatedLoRaDriver() : sock_fd_(-1) {
    const char* port_env = std::getenv("SIM_LORA_PORT");
    port_ = port_env ? std::atoi(port_env) : 5000;

    const char* neighbors_env = std::getenv("SIM_LORA_NEIGHBORS");
    if (neighbors_env) {
        std::stringstream ss(neighbors_env);
        std::string neighbor;
        while (std::getline(ss, neighbor, ',')) {
            neighbors_.push_back(neighbor);
        }
    }
}

SimulatedLoRaDriver::~SimulatedLoRaDriver() {
    if (sock_fd_ != -1) {
        close(sock_fd_);
    }
}

std::expected<void, std::error_code> SimulatedLoRaDriver::initialize() {
    if ((sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return std::unexpected(std::make_error_code(std::errc::io_error));
    }

    memset(&servaddr_, 0, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_addr.s_addr = INADDR_ANY;
    servaddr_.sin_port = htons(port_);

    if (bind(sock_fd_, (const struct sockaddr *)&servaddr_, sizeof(servaddr_)) < 0) {
        return std::unexpected(std::make_error_code(std::errc::address_in_use));
    }

    std::cout << "[SimulatedLoRa] Initialized on port " << port_ << " with " << neighbors_.size() << " neighbors.\n";
    return {};
}

std::expected<void, std::error_code> SimulatedLoRaDriver::send(std::span<const uint8_t> data) {
    if (sock_fd_ == -1) return std::unexpected(std::make_error_code(std::errc::not_connected));

    for (const auto& neighbor : neighbors_) {
        struct sockaddr_in dest;
        memset(&dest, 0, sizeof(dest));
        dest.sin_family = AF_INET;
        dest.sin_port = htons(port_);
        
        struct hostent *he = gethostbyname(neighbor.c_str());
        if (he) {
            memcpy(&dest.sin_addr, he->h_addr_list[0], he->h_length);
            ssize_t sent = sendto(sock_fd_, data.data(), data.size(), 0, (const struct sockaddr *)&dest, sizeof(dest));
            if (sent < 0) {
                std::cerr << "[SimulatedLoRa] Failed to send to " << neighbor << ": " << strerror(errno) << "\n";
            }
        } else {
            std::cerr << "[SimulatedLoRa] Could not resolve neighbor: " << neighbor << "\n";
        }
    }
    return {};
}

std::expected<size_t, std::error_code> SimulatedLoRaDriver::receive(std::span<uint8_t> buffer) {
    if (sock_fd_ == -1) return std::unexpected(std::make_error_code(std::errc::not_connected));

    struct sockaddr_in src;
    socklen_t len = sizeof(src);
    ssize_t n = recvfrom(sock_fd_, buffer.data(), buffer.size(), 0, (struct sockaddr *)&src, &len);
    
    if (n < 0) {
        return std::unexpected(std::make_error_code(std::errc::io_error));
    }

    char sender_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &src.sin_addr, sender_ip, INET_ADDRSTRLEN);

    return static_cast<size_t>(n);
}

} // namespace guardian::telemetry
