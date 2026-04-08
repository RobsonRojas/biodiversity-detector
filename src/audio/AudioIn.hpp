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
     * @return guardian::expected<void, std::error_code> success or error.
     */
    guardian::expected<void, std::error_code> open();

    /**
     * @brief Reads samples from the device.
     * @param buffer span to fill with INT32 samples.
     * @return guardian::expected<size_t, std::error_code> number of samples read or error.
     */
    guardian::expected<size_t, std::error_code> read(guardian::span<int32_t> buffer);

    /**
     * @brief Reads 16-bit samples from the device.
     * @param buffer span to fill with INT16 samples.
     * @return guardian::expected<size_t, std::error_code> number of samples read or error.
     */
    guardian::expected<size_t, std::error_code> read_int16(guardian::span<int16_t> buffer);

    void close();

private:
    std::string device_path_;
    int fd_;
};

} // namespace guardian::audio
