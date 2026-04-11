#include "esp_lora_driver.hpp"
#include "esp_log.h"
#include <string.h>

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
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = LORA_MOSI_IO,
        .miso_io_num = LORA_MISO_IO,
        .sclk_io_num = LORA_SCK_IO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    // 2. Device Configuration
    spi_device_interface_config_t dev_cfg = {
        .mode = 0,
        .clock_speed_hz = 10 * 1000 * 1000, // 10MHz
        .spics_io_num = LORA_CS_IO,
        .queue_size = 7,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_handle_));

    // 3. GPIO Configuration
    gpio_config_t rst_cfg = {
        .pin_bit_mask = (1ULL << LORA_RST_IO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&rst_cfg);

    gpio_config_t busy_cfg = {
        .pin_bit_mask = (1ULL << LORA_BUSY_IO) | (1ULL << LORA_DIO1_IO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
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
    // Check DIO1 for received packets
}

int EspLoRaDriver::get_last_rssi() {
    // In a real implementation, we would query the SX126x/SX127x for the last packet's RSSI.
    // For now, return the last stored RSSI value.
    return last_rssi_;
}

void EspLoRaDriver::write_reg(uint8_t addr, uint8_t val) {
    uint8_t tx_buf[2] = { (uint8_t)(addr | 0x80), val };
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx_buf,
    };
    spi_device_polling_transmit(spi_handle_, &t);
}

uint8_t EspLoRaDriver::read_reg(uint8_t addr) {
    uint8_t tx_buf[2] = { addr, 0x00 };
    uint8_t rx_buf[2];
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf,
    };
    spi_device_polling_transmit(spi_handle_, &t);
    return rx_buf[1];
}
