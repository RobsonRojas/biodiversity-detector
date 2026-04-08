#pragma once

#include <vector>
#include <cstdint>

namespace guardian::hal {

class I2SCapture {
public:
    /**
     * @brief Initialize I2S peripheral with DMA.
     * @param sample_rate Audio sampling rate (e.g., 16000).
     */
    static void init(uint32_t sample_rate);

    /**
     * @brief Read samples into a buffer.
     * @param buffer Target buffer to fill.
     * @return Number of samples read.
     */
    static size_t read(std::vector<int16_t>& buffer);

    /**
     * @brief Stop I2S and DMA.
     */
    static void deinit();
};

} // namespace guardian::hal
