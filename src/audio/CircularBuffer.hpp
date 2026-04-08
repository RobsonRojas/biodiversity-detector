#pragma once
#include "../utils/compat.hpp"

#include <vector>
#include <mutex>
#include <algorithm>

namespace guardian::audio {

/**
 * @brief Thread-safe circular buffer for audio samples.
 */
class CircularBuffer {
public:
    explicit CircularBuffer(size_t capacity) : buffer_(capacity), head_(0), size_(0) {}

    void push(guardian::span<const int32_t> samples) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto s : samples) {
            buffer_[head_] = s;
            head_ = (head_ + 1) % buffer_.size();
            size_ = std::min(size_ + 1, buffer_.size());
        }
    }

    /**
     * @brief Reads the last N samples from the buffer.
     * @param target span to fill with the most recent samples.
     */
    void read_latest(guardian::span<int32_t> target) {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t n = target.size();
        if (n > size_) n = size_;

        for (size_t i = 0; i < n; ++i) {
            size_t idx = (head_ + buffer_.size() - n + i) % buffer_.size();
            target[i] = buffer_[idx];
        }
    }

    size_t size() const { return size_; }

private:
    std::vector<int32_t> buffer_;
    size_t head_;
    size_t size_;
    std::mutex mutex_;
};

} // namespace guardian::audio
