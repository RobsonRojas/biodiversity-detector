/*
 * Copyright (C) 2026 Robson Rojas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "memory_helpers.hpp"
#include <atomic>
#include <stddef.h>
#include <stdint.h>

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
  void *get_free_segment_ptr();
  void commit_segment();

  // Zero-copy access for the consumer (DSP/AI)
  const void *get_ready_segment_ptr();
  const SegmentMetadata *get_ready_segment_metadata();
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
