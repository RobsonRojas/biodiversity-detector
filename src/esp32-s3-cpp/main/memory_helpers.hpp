#pragma once

#include <memory>
#include "esp_heap_caps.h"

/**
 * @brief Helper types and functions for RAII memory management in ESP32 hardware drivers.
 */

namespace memory {

/**
 * @brief Custom deleter for DMA-capable memory allocated via heap_caps_malloc.
 */
struct HeapCapsDeleter {
    void operator()(void* p) const {
        if (p) {
            heap_caps_free(p);
        }
    }
};

/**
 * @brief Standardized unique_ptr for DMA-managed arrays.
 * Ensures heap_caps_free is used instead of delete[].
 */
template<typename T>
using DmaUniquePtr = std::unique_ptr<T[], HeapCapsDeleter>;

/**
 * @brief Factory function to allocate DMA-capable memory with RAII ownership.
 * @param count Number of elements of type T
 * @param caps Capability flags (defaults to DMA | INTERNAL)
 * @return DmaUniquePtr<T> Managed pointer
 */
template<typename T>
DmaUniquePtr<T> make_dma_unique(size_t count, uint32_t caps = MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL) {
    void* ptr = heap_caps_malloc(count * sizeof(T), caps);
    return DmaUniquePtr<T>(static_cast<T*>(ptr));
}

} // namespace memory
