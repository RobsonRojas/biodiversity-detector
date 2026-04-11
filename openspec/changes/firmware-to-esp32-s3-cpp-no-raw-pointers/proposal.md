## Why

The current firmware implementation uses raw pointers and manual `malloc`/`free` calls for large buffers (FFT, DMA, RingBuffer). This increases the risk of memory leaks, double-free errors, and use-after-free vulnerabilities, especially as the codebase grows with more biodiversity detection features. Moving to C++ smart pointers (RAII) will ensure predictable resource cleanup and improve overall system stability.

## What Changes

- **Smart Pointer Migration**: Replace `malloc`/`free` with `std::unique_ptr` and `std::shared_ptr` across all components.
- **RAII DMA Buffers**: Implement custom deleters for `std::unique_ptr` when managing DMA-capable memory (using `heap_caps_free`).
- **Component Lifecycle**: Refactor `DspAnalyzer`, `SharedRingBuffer`, and `I2SLowLevel` to use smart pointers for internal buffer management.
- **Dependency Cleanup**: Fix missing `esp_timer` and `esp_driver_spi` requirements in `CMakeLists.txt` to ensure stable builds.

## Capabilities

### New Capabilities
- `managed-memory-firmware`: Enforces the use of C++ smart pointers for all dynamic memory allocations in the ESP32-S3 firmware.

### Modified Capabilities
- `i2s-high-fidelity-capture`: Update the internal implementation of the I2S driver to use smart pointers for DMA descriptor management.
- `zero-copy-audio-transfer`: Update the `SharedRingBuffer` to manage its memory pool using RAII.

## Impact

- **Memory Safety**: Dramatically reduces the risk of leaks in long-running deployments.
- **Code Quality**: Simplifies cleanup logic in destructors and error paths.
- **Build System**: Improves build reliability by correctly declaring all hardware driver dependencies.
