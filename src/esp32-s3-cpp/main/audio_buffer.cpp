#include "audio_buffer.hpp"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "AUDIO_BUFFER";

AudioBuffer::AudioBuffer(size_t capacity) : capacity_(capacity), count_(0) {
    // Allocate in SPIRAM (PSRAM)
    samples_ = (float*)heap_caps_malloc(capacity * sizeof(float), MALLOC_CAP_SPIRAM);
    if (!samples_) {
        ESP_LOGE(TAG, "Failed to allocate %zu samples in PSRAM!", capacity);
    } else {
        memset(samples_, 0, capacity * sizeof(float));
        ESP_LOGI(TAG, "Allocated %zu samples in PSRAM.", capacity);
    }
}

AudioBuffer::~AudioBuffer() {
    if (samples_) {
        free(samples_);
    }
}

void AudioBuffer::push(const float* samples, size_t count) {
    if (!samples_) return;

    if (count >= capacity_) {
        // Direct copy of the last part
        memcpy(samples_, samples + count - capacity_, capacity_ * sizeof(float));
        count_ = capacity_;
    } else {
        // Shift existing samples left
        memmove(samples_, samples_ + count, (capacity_ - count) * sizeof(float));
        // Copy new samples to the end
        memcpy(samples_ + capacity_ - count, samples, count * sizeof(float));
        
        if (count_ < capacity_) {
            count_ += count;
            if (count_ > capacity_) count_ = capacity_;
        }
    }
}
