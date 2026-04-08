#include "../utils/compat.hpp"
#include "AudioIn.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

namespace guardian::audio {

AudioIn::AudioIn(std::string device_path) : device_path_(std::move(device_path)), fd_(-1) {}

AudioIn::~AudioIn() {
    close();
}

guardian::expected<void, std::error_code> AudioIn::open() {
    fd_ = ::open(device_path_.c_str(), O_RDONLY);
    if (fd_ < 0) {
        return guardian::unexpected(std::error_code(errno, std::system_category()));
    }
    return {};
}

guardian::expected<size_t, std::error_code> AudioIn::read(guardian::span<int32_t> buffer) {
    if (fd_ < 0) {
        return guardian::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    }

    ssize_t bytes_read = ::read(fd_, buffer.data(), buffer.size_bytes());
    if (bytes_read < 0) {
        return guardian::unexpected(std::error_code(errno, std::system_category()));
    }

    return static_cast<size_t>(bytes_read / sizeof(int32_t));
}

guardian::expected<size_t, std::error_code> AudioIn::read_int16(guardian::span<int16_t> buffer) {
    if (fd_ < 0) {
        return guardian::unexpected(std::make_error_code(std::errc::bad_file_descriptor));
    }

    ssize_t bytes_read = ::read(fd_, buffer.data(), buffer.size_bytes());
    if (bytes_read < 0) {
        return guardian::unexpected(std::error_code(errno, std::system_category()));
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
