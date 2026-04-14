## Context

The ESP32-S3 firmware currently manages heap allocation for DSP buffers and peripheral drivers using raw pointers and manual lifecycle management (`malloc`/`free`, `heap_caps_malloc`). This approach is error-prone and can lead to fragmentation or memory leaks in long-running biodiversity monitoring deployments. We need to transition to Modern C++ (C++20/23) paradigms using RAII (Resource Acquisition Is Initialization) and smart pointers.

## Goals / Non-Goals

**Goals:**
- Eliminate manual `free()` and `heap_caps_free()` calls from component code.
- Ensure 100% cleanup of dynamic resources upon component destruction.
- Stabilize the build system by correctly declaring driver dependencies in `CMakeLists.txt`.
- Maintain the zero-copy performance of the audio capture pipeline.

**Non-Goals:**
- Porting the code to a different framework (remain on ESP-IDF).
- Refactoring the DSP math logic or inference engine.

## Decisions

### 1. Unique Ownership for DSP Buffers
**Decision**: Use `std::unique_ptr<float[]>` for FFT windows and buffers.
**Rationale**: These buffers are exclusively owned by the `DspAnalyzer` instance. Smart pointers eliminate the need for cleanup code in the destructor.

### 2. Custom Deleters for DMA Memory
**Decision**: Use `std::unique_ptr<T, void(*)(void*)>` with a custom deleter wrapping `heap_caps_free`.
**Rationale**: Memory allocated with `heap_caps_malloc` (required for DMA/Internal RAM on S3) must be freed with `heap_caps_free`. A custom deleter ensures the correct free function is called.

### 3. Smart Pointer Management in `AudioCapture`
**Decision**: Refactor `AudioCapture` to manage its `I2SLowLevel` and `SharedRingBuffer` using `std::unique_ptr`.
**Rationale**: Allows for flexible initialization and ensures the I2S hardware driver is stopped and cleaned up automatically if the capture object is destroyed.

### 4. Build System Refactoring
**Decision**: Add `esp_timer` and `esp_driver_spi` to the `main` component requirements.
**Rationale**: These are required for high-resolution timing and LoRa communication but were missing from the component registry, causing build failures.

## Risks / Trade-offs

- **[Risk]** Overhead of smart pointer initialization → **[Mitigation]** Smart pointers (`unique_ptr`) have zero runtime overhead compared to raw pointers once initialized. We will avoid allocations in the hot processing loop.
- **[Risk]** Complexity of custom deleters for arrays → **[Mitigation]** Define a helper alias or factory function for DMA-managed smart pointers.
