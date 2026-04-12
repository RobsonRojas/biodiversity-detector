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

#include "esp_lora_driver.hpp"
#include "esp_log.h"
#include "driver/uart.h"
#include <string.h>
#include <string>
#include <vector>
#include <sstream>

static const char *TAG = "LORA_DRIVER";

// Defaults for S3-DevKitC
#define LORA_SCK_IO   12
#define LORA_MISO_IO  13
#define LORA_MOSI_IO  11
#define LORA_CS_IO    10
#define LORA_RST_IO   14
#define LORA_DIO1_IO  39
#define LORA_BUSY_IO  21

EspLoRaDriver::EspLoRaDriver() : spi_handle_(NULL), last_rssi_(-120) {}

EspLoRaDriver::~EspLoRaDriver() {
    if (spi_handle_) {
        spi_bus_remove_device(spi_handle_);
    }
}

esp_err_t EspLoRaDriver::init() {
    // 1. SPI Bus Configuration
    spi_bus_config_t bus_cfg = {};
    bus_cfg.mosi_io_num = LORA_MOSI_IO;
    bus_cfg.miso_io_num = LORA_MISO_IO;
    bus_cfg.sclk_io_num = LORA_SCK_IO;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
    bus_cfg.max_transfer_sz = 0;
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    // 2. Device Configuration
    spi_device_interface_config_t dev_cfg = {};
    dev_cfg.mode = 0;
    dev_cfg.clock_speed_hz = 10 * 1000 * 1000; // 10MHz
    dev_cfg.spics_io_num = LORA_CS_IO;
    dev_cfg.queue_size = 7;
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_handle_));

    // 3. GPIO Configuration
    gpio_config_t rst_cfg = {};
    rst_cfg.pin_bit_mask = (1ULL << LORA_RST_IO);
    rst_cfg.mode = GPIO_MODE_OUTPUT;
    rst_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
    rst_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    rst_cfg.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&rst_cfg);

    gpio_config_t busy_cfg = {};
    busy_cfg.pin_bit_mask = (1ULL << LORA_BUSY_IO) | (1ULL << LORA_DIO1_IO);
    busy_cfg.mode = GPIO_MODE_INPUT;
    busy_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
    busy_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    busy_cfg.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&busy_cfg);

    ESP_LOGI(TAG, "LoRa SPI initialized on S3 (Standard Pinout)");
    return ESP_OK;
}

esp_err_t EspLoRaDriver::send(const uint8_t* data, size_t len) {
    ESP_LOGI(TAG, "Sending LoRa Packet: %zu bytes", len);
    // TODO: Implement SX126x/SX127x specific command sequence (SetBufferBase, WriteBuffer, SetTx)
    return ESP_OK;
}

void EspLoRaDriver::poll() {
    // Stage 1: Check for Virtual LoRa packets via UART 0 (Console bridge)
    uint8_t data[256];
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM_0, (size_t*)&length));
    if (length > 0) {
        length = uart_read_bytes(UART_NUM_0, data, length, 10 / portTICK_PERIOD_MS);
        if (length > 0) {
            std::string line((char*)data, length);
            if (line.find("LORA_RX:") == 0) {
                // Protocol: LORA_RX:PAYLOAD:RSSI\n
                size_t first_colon = line.find(':', 8);
                if (first_colon != std::string::npos) {
                    std::string payload = line.substr(8, first_colon - 8);
                    std::string rssi_str = line.substr(first_colon + 1);
                    last_rssi_ = std::stoi(rssi_str);
                    
                    ESP_LOGI(TAG, "Virtual LoRa RX: %s (RSSI: %d)", payload.c_str(), last_rssi_);
                    
                    if (localization_engine_ && payload.find("BEACON:") == 0) {
                        // BEACON:ID:LAT:LON:ACC
                        std::vector<std::string> parts;
                        std::stringstream pss(payload);
                        std::string part;
                        while (std::getline(pss, part, ':')) {
                            parts.push_back(part);
                        }

                        if (parts.size() >= 5) {
                            uint8_t id = (uint8_t)strtol(parts[1].c_str(), NULL, 10);
                            NodeCoords coords;
                            coords.lat = strtod(parts[2].c_str(), NULL);
                            coords.lon = strtod(parts[3].c_str(), NULL);
                            coords.accuracy = strtod(parts[4].c_str(), NULL);
                            localization_engine_->update_neighbor(id, coords, last_rssi_);
                            
                            // Trigger position recalculation
                            if (localization_engine_->calculate_position()) {
                                ESP_LOGI(TAG, "Localization Updated! New coords: %f, %f", 
                                         localization_engine_->get_current_coords().lat,
                                         localization_engine_->get_current_coords().lon);
                            }
                        }
                    }
                }
            }
        }
    }

    // Stage 2: Hardware Polling (Original Logic)
    // 1. Check DIO1 for received packets
    // ...
}

int EspLoRaDriver::get_last_rssi() {
    // Query the SX126x/SX127x hardware for the last packet's RSSI.
    // For SX126x: Command GetPacketStatus (0x14) returns RSSI Pkt in dBm.
    // For SX127x: Read RegRssiValue (0x1B) or RegPktRssiValue (0x1A).
    
    // In this implementation, we return the cached value updated during poll().
    return last_rssi_;
}

void EspLoRaDriver::write_reg(uint8_t addr, uint8_t val) {
    uint8_t tx_buf[2] = { (uint8_t)(addr | 0x80), val };
    spi_transaction_t t = {};
    t.length = 16;
    t.tx_buffer = tx_buf;
    spi_device_polling_transmit(spi_handle_, &t);
}

uint8_t EspLoRaDriver::read_reg(uint8_t addr) {
    uint8_t tx_buf[2] = { addr, 0x00 };
    uint8_t rx_buf[2];
    spi_transaction_t t = {};
    t.length = 16;
    t.tx_buffer = tx_buf;
    t.rx_buffer = rx_buf;
    spi_device_polling_transmit(spi_handle_, &t);
    return rx_buf[1];
}
