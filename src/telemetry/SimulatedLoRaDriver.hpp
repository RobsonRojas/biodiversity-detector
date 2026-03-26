#pragma once

#include "LoRaDriver.hpp"
#include "../utils/compat.hpp"
#include <netinet/in.h>
#include <string>
#include <vector>

namespace guardian::telemetry {

class SimulatedLoRaDriver : public LoRaDriver {
public:
    SimulatedLoRaDriver();
    ~SimulatedLoRaDriver();

    std::expected<void, std::error_code> initialize();
    std::expected<void, std::error_code> send(std::span<const uint8_t> data);
    std::expected<size_t, std::error_code> receive(std::span<uint8_t> buffer);

private:
    int sock_fd_;
    int port_;
    std::vector<std::string> neighbors_;
    struct sockaddr_in servaddr_;
};

} // namespace guardian::telemetry
