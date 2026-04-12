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

#include "esp_err.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <cstdint>
#include <vector>
#include <string>

/**
 * @brief Native ESP-IDF LoRa driver for SX126x/SX127x chips.
 * Implements basic send/receive for the Guardian mesh network.
 */
class EspLoRaDriver {
public:
    EspLoRaDriver();
    ~EspLoRaDriver();

    /**
     * @brief Initialize the SPI bus and GPIO pins for the LoRa chip
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t init();

    /**
     * @brief Send an alert packet over the LoRa mesh
     * @param data Pointer to the packet data
     * @param len Length of the packet
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t send(const uint8_t* data, size_t len);

    /**
     * @brief Poll for incoming packets or IRQ status
     */
    void poll();

    /**
     * @brief Get the RSSI of the last received/sent packet
     * @return int RSSI in dBm
     */
    int get_last_rssi();

    /**
     * @brief Set the localization engine to update on neighbor beacons
     */
    void set_localization_engine(class LocalizationEngine* engine) { localization_engine_ = engine; }

private:
    spi_device_handle_t spi_handle_;
    int last_rssi_;
    class LocalizationEngine* localization_engine_;
    
    // Low-level SPI operations
    void write_reg(uint8_t addr, uint8_t val);
    uint8_t read_reg(uint8_t addr);
};
