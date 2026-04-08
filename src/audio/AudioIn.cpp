#include "AudioIn.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

namespace guardian::audio {

AudioIn::AudioIn(std::string device_path) : device_path_(std::move(device_path)), fd_(-1) {}

AudioIn::~AudioIn() {
    close();
}

std::expected<void, std::error_code> AudioIn::open() {
    fd_ = ::open(device_path_.c_str(), O_RDONLY);
    if (fd_ < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }
    return {};
}

std::expected<size_t, std::error_code> AudioIn::read(std::span<int32_t> buffer) {
    if (fd_ < 0) {
        return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    }

    ssize_t bytes_read = ::read(fd_, buffer.data(), buffer.size_bytes());
    if (bytes_read < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return static_cast<size_t>(bytes_read / sizeof(int32_t));
}

std::expected<size_t, std::error_code> AudioIn::read_int16(std::span<int16_t> buffer) {
    if (fd_ < 0) {
        return std::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    }

    ssize_t bytes_read = ::read(fd_, buffer.data(), buffer.size_bytes());
    if (bytes_read < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return static_cast<size_t>(bytes_read / sizeof(int16_t));
}

void AudioIn::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

} // namespace guardian::audio
