#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <expected>
#include <system_error>

namespace guardian::telemetry {

class LoRaDriver {
public:
    LoRaDriver();
    ~LoRaDriver();

    std::expected<void, std::error_code> initialize();

    // 1.2 Implement a buffer manager using `std::span` for zero-copy packet handling.
    std::expected<void, std::error_code> send(std::span<const uint8_t> data);
    std::expected<size_t, std::error_code> receive(std::span<uint8_t> buffer);

private:
    int fd_;
};

} // namespace guardian::telemetry
