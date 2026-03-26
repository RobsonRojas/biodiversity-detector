#pragma once

#include "LoRaDriver.hpp"
#include <linux/spi/spidev.h>
#include <string>

namespace guardian::telemetry {

class PhysicalLoRaDriver : public LoRaDriver {
public:
    PhysicalLoRaDriver(const std::string& spi_dev, int nss_pin, int busy_pin, int dio1_pin, int rst_pin);
    ~PhysicalLoRaDriver() override;

    std::expected<void, std::error_code> initialize() override;
    std::expected<void, std::error_code> send(std::span<const uint8_t> data) override;
    std::expected<size_t, std::error_code> receive(std::span<uint8_t> buffer) override;

private:
    void reset();
    void wait_busy();
    void write_command(uint8_t op, std::span<const uint8_t> data);
    void read_command(uint8_t op, std::span<uint8_t> data);

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
