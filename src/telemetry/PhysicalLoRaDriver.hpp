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
