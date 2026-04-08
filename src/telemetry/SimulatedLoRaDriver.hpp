#pragma once

#include "LoRaDriver.hpp"
#include "../utils/compat.hpp"
#include <netinet/in.h>
#include <string>
#include <vector>

namespace guardian::telemetry {

class SimulatedLoRaDriver : public LoRaDriver {
public:
    SimulatedLoRaDriver(int port, const std::vector<std::string>& neighbors);
    ~SimulatedLoRaDriver() override;

    guardian::expected<void, std::error_code> initialize() override;
    guardian::expected<void, std::error_code> send(guardian::span<const uint8_t> data) override;
    guardian::expected<size_t, std::error_code> receive(guardian::span<uint8_t> buffer) override;

private:
    int sock_fd_;
    int port_;
    std::vector<std::string> neighbors_;
    struct sockaddr_in servaddr_;
};

} // namespace guardian::telemetry
