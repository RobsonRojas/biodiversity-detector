#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <system_error>
#include "../utils/compat.hpp"

namespace guardian::telemetry {

class LoRaDriver {
public:
    LoRaDriver();
    virtual ~LoRaDriver();

    virtual guardian::expected<void, std::error_code> initialize();

    // 1.2 Implement a buffer manager using `guardian::span` for zero-copy packet handling.
    virtual guardian::expected<void, std::error_code> send(guardian::span<const uint8_t> data);
    virtual guardian::expected<size_t, std::error_code> receive(guardian::span<uint8_t> buffer);

private:
    int fd_;
};

} // namespace guardian::telemetry
