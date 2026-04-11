#pragma once

#include <stdint.h>
#include <stddef.h>
#include <atomic>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "memory_helpers.hpp"

#define RING_BUFFER_MAX_SEGMENTS 4
#define RING_BUFFER_SEGMENT_SIZE 1024 // Bytes

struct SegmentMetadata {
    uint64_t timestamp_us;
    size_t length;
};

class SharedRingBuffer {
public:
    SharedRingBuffer();
    ~SharedRingBuffer();

    // Zero-copy access for the producer (DMA)
    void* get_free_segment_ptr();
    void commit_segment();

    // Zero-copy access for the consumer (DSP/AI)
    const void* get_ready_segment_ptr();
    const SegmentMetadata* get_ready_segment_metadata();
    void release_segment();

    int get_occupancy();

    bool has_data();
    bool has_space();

private:
    memory::DmaUniquePtr<uint8_t> buffer;
    size_t total_size;
    
    std::atomic<int> write_index;
    std::atomic<int> read_index;
    std::atomic<int> count;

    SegmentMetadata metadata[RING_BUFFER_MAX_SEGMENTS];
};
