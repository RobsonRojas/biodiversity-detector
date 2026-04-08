#pragma once
#include "../utils/compat.hpp"

#include "LoRaDriver.hpp"
#include <linux/spi/spidev.h>
#include <string>

namespace guardian::telemetry {

class PhysicalLoRaDriver : public LoRaDriver {
public:
    PhysicalLoRaDriver(const std::string& spi_dev, int nss_pin, int busy_pin, int dio1_pin, int rst_pin);
    ~PhysicalLoRaDriver() override;

    guardian::expected<void, std::error_code> initialize() override;
    guardian::expected<void, std::error_code> send(guardian::span<const uint8_t> data) override;
    guardian::expected<size_t, std::error_code> receive(guardian::span<uint8_t> buffer) override;

private:
    void reset();
    void wait_busy();
    void write_command(uint8_t op, guardian::span<const uint8_t> data);
    void read_command(uint8_t op, guardian::span<uint8_t> data);

    std::string spi_dev_path_;
    int spi_fd_ = -1;
    int nss_pin_;
    int busy_pin_;
    int dio1_pin_;
    int rst_pin_;
    
    // GPIO handles (will use lgpio if available)
    int h_gpio_ = -1; 
};

} // namespace guardian::telemetry
