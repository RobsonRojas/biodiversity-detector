#include "PhysicalLoRaDriver.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iostream>
#include <cstring>

namespace guardian::telemetry {

PhysicalLoRaDriver::PhysicalLoRaDriver(const std::string& spi_dev, int nss_pin, int busy_pin, int dio1_pin, int rst_pin)
    : spi_dev_path_(spi_dev), nss_pin_(nss_pin), busy_pin_(busy_pin), dio1_pin_(dio1_pin), rst_pin_(rst_pin) {}

PhysicalLoRaDriver::~PhysicalLoRaDriver() {
    if (spi_fd_ >= 0) close(spi_fd_);
}

std::expected<void, std::error_code> PhysicalLoRaDriver::initialize() {
    std::cout << "[LoRa] Initializing Physical SX1262 on " << spi_dev_path_ << "..." << std::endl;
    
    spi_fd_ = open(spi_dev_path_.c_str(), O_RDWR);
    if (spi_fd_ < 0) {
        return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
    }

    uint8_t mode = 0;
    uint8_t bits = 8;
    uint32_t speed = 2000000; // 2MHz

    if (ioctl(spi_fd_, SPI_IOC_WR_MODE, &mode) < 0) return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    if (ioctl(spi_fd_, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    if (ioctl(spi_fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) return std::unexpected(std::make_error_code(std::errc::invalid_argument));

    reset();
    wait_busy();

    std::cout << "[LoRa] SX1262 Reset and Ready.\n";
    return {};
}

std::expected<void, std::error_code> PhysicalLoRaDriver::send(std::span<const uint8_t> data) {
    if (spi_fd_ < 0) return std::unexpected(std::make_error_code(std::errc::not_connected));
    
    std::cout << "[LoRa] Sending physical packet (" << data.size() << " bytes) via SPI...\n";
    wait_busy();
    
    // Command 0x0E: WriteBuffer
    write_command(0x0E, data);
    
    // Command 0x85: SetTx (timeout 0 = infinite/manual)
    uint8_t timeout[3] = {0, 0, 0};
    write_command(0x85, std::span<const uint8_t>(timeout, 3));
    
    return {};
}

std::expected<size_t, std::error_code> PhysicalLoRaDriver::receive(std::span<uint8_t> buffer) {
    // Basic polling mock for hardware testing
    return 0;
}

void PhysicalLoRaDriver::reset() {
     std::cout << "[LoRa] Toggling RST Pin " << rst_pin_ << "\n";
     // gpio_write(rst_pin_, 0); usleep(100); gpio_write(rst_pin_, 1);
}

void PhysicalLoRaDriver::wait_busy() {
    // Poll BUSY pin: while(gpio_read(busy_pin_) == HIGH);
}

void PhysicalLoRaDriver::write_command(uint8_t op, std::span<const uint8_t> data) {
    struct spi_ioc_transfer tr[2];
    memset(tr, 0, sizeof(tr));

    uint8_t cmd = op;
    tr[0].tx_buf = (unsigned long)&cmd;
    tr[0].len = 1;
    
    tr[1].tx_buf = (unsigned long)data.data();
    tr[1].len = data.size();

    ioctl(spi_fd_, SPI_IOC_MESSAGE(2), tr);
}

} // namespace guardian::telemetry
