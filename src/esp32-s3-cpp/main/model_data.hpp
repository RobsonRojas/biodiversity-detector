#pragma once

/**
 * @file model_data.hpp
 * @brief Embedded TFLite model binary for chainsaw / biodiversity classification.
 *
 * HOW TO UPDATE:
 *   1. Train a model in Edge Impulse Studio → Export as C++ library.
 *   2. Locate the quantized .tflite file inside the exported archive.
 *   3. Convert to a C array with: xxd -i model.tflite > model_data.cc
 *   4. Replace the g_model_data array below with the generated array.
 *   5. Update g_model_data_len accordingly.
 *
 * The placeholder below is a minimal valid TFLite FlatBuffer that will
 * parse without crashing. It does NOT produce meaningful outputs.
 * Replace it with the real exported model before field deployment.
 */

#include <cstdint>
#include <cstddef>

// --- Classification Labels ---------------------------------------------------
// These MUST match the order of the output tensor from your Edge Impulse model.
static const char* const kCategoryLabels[] = {
    "Chainsaw",
    "Animal",
    "Insect",
    "Background"
};
static constexpr int kCategoryCount = sizeof(kCategoryLabels) / sizeof(kCategoryLabels[0]);

// --- Tensor‑arena sizing (tuned for ESP32‑S3 PSRAM) --------------------------
// Start with 64 KiB; increase if the allocator fails at interpreter creation.
static constexpr size_t kTensorArenaSize = 64 * 1024;

// --- Model binary (placeholder) ----------------------------------------------
// IMPORTANT: Replace this array with the real model exported from Edge Impulse.
// A minimal valid FlatBuffer schema header so TFLite Micro can at least parse
// the buffer identifier ("TFL3") without a hard fault.
alignas(16) static const uint8_t g_model_data[] = {
    // Minimal FlatBuffer with "TFL3" identifier (24 bytes).
    // Offset 0: root‑table offset (little‑endian uint32)
    0x10, 0x00, 0x00, 0x00,
    // Bytes 4‑7: file identifier "TFL3"
    0x54, 0x46, 0x4C, 0x33,
    // Bytes 8‑15: padding / vtable pointer
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // Bytes 16‑23: empty root table
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static constexpr size_t g_model_data_len = sizeof(g_model_data);
