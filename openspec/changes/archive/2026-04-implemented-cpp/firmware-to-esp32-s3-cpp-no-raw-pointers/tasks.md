## 1. Core Memory Management Utilities

- [ ] 1.1 Implement helper type aliases or factory functions for DMA-capable `std::unique_ptr` with custom deleters.
- [ ] 1.2 Add necessary includes (`<memory>`) to common headers.

## 2. Component Refactoring (RAII)

- [ ] 2.1 Refactor `DspAnalyzer` to use `std::unique_ptr` for `window`, `fft_input`, and `fft_output` buffers. Correctly remove `free()` calls in the destructor.
- [ ] 2.2 Refactor `SharedRingBuffer` to manage the main DMA buffer pool using a managed smart pointer.
- [ ] 2.3 Refactor `I2SLowLevel` to use `std::unique_ptr` for the DMA descriptor chain allocation.
- [ ] 2.4 Update `AudioCapture` to manage its internal driver and buffer instances using smart pointers if applicable or verify stack-ownership safety.

## 3. Integration and Build Fixes

- [ ] 3.1 Update `main/CMakeLists.txt` to include `esp_timer` and `esp_driver_spi` in the component requirements.
- [ ] 3.2 Update `main.cpp` to utilize smart pointer accessors and verify component lifetimes.
- [ ] 3.3 Remove any remaining manual `malloc`/`free` or `heap_caps_malloc`/`free` calls in the application layer.

## 4. Verification

- [x] 4.1 Validate OpenSpec change `firmware-to-esp32-s3-cpp-no-raw-pointers`.
- [ ] 4.2 Run a build check through the Docker container to ensure all dependencies are resolved.
- [ ] 4.3 Verify stable execution and memory integrity (no leaks during rapid object creation/destruction).
