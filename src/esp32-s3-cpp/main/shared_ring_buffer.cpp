#include "shared_ring_buffer.hpp"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include <string.h>

SharedRingBuffer::SharedRingBuffer() : write_index(0), read_index(0), count(0) {
    total_size = RING_BUFFER_MAX_SEGMENTS * RING_BUFFER_SEGMENT_SIZE;
    // Allocate DRAM for DMA using RAII
    buffer = memory::make_dma_unique<uint8_t>(total_size);
    if (buffer) {
        memset(buffer.get(), 0, total_size);
    }
}

SharedRingBuffer::~SharedRingBuffer() = default;

void* SharedRingBuffer::get_free_segment_ptr() {
    if (!has_space()) return NULL;
    return buffer.get() + (write_index.load() * RING_BUFFER_SEGMENT_SIZE);
}

void SharedRingBuffer::commit_segment() {
    int idx = write_index.load();
    metadata[idx].timestamp_us = esp_timer_get_time();
    metadata[idx].length = RING_BUFFER_SEGMENT_SIZE;
    
    write_index.store((idx + 1) % RING_BUFFER_MAX_SEGMENTS);
    count++;
}

const void* SharedRingBuffer::get_ready_segment_ptr() {
    if (!has_data()) return NULL;
    return buffer.get() + (read_index.load() * RING_BUFFER_SEGMENT_SIZE);
}

const SegmentMetadata* SharedRingBuffer::get_ready_segment_metadata() {
    if (!has_data()) return NULL;
    return &metadata[read_index.load()];
}

void SharedRingBuffer::release_segment() {
    int idx = read_index.load();
    read_index.store((idx + 1) % RING_BUFFER_MAX_SEGMENTS);
    count--;
}

int SharedRingBuffer::get_occupancy() {
    return count;
}

bool SharedRingBuffer::has_data() {
    return count > 0;
}

bool SharedRingBuffer::has_space() {
    return count < RING_BUFFER_MAX_SEGMENTS;
}
