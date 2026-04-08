#pragma once

#include <string>
#include <system_error>
#include <vector>
#include "../utils/compat.hpp"

namespace guardian::audio {

/**
 * @brief Class to interface with the /dev/forest_audio device.
 */
class AudioIn {
public:
    explicit AudioIn(std::string device_path = "/dev/forest_audio");
    ~AudioIn();

    /**
     * @brief Opens the device node.
     * @return std::expected<void, std::error_code> success or error.
     */
    std::expected<void, std::error_code> open();

    /**
     * @brief Reads samples from the device.
     * @param buffer span to fill with INT32 samples.
     * @return std::expected<size_t, std::error_code> number of samples read or error.
     */
    std::expected<size_t, std::error_code> read(std::span<int32_t> buffer);

    /**
     * @brief Reads 16-bit samples from the device.
     * @param buffer span to fill with INT16 samples.
     * @return std::expected<size_t, std::error_code> number of samples read or error.
     */
    std::expected<size_t, std::error_code> read_int16(std::span<int16_t> buffer);

    void close();

private:
    std::string device_path_;
    int fd_;
};

} // namespace guardian::audio
