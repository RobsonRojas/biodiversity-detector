## Why

The current firmware implementation uses Rust (`src/esp32`). While Rust provides excellent safety guarantees, the specific requirements for ESP32-S3 deployment in this project—including high-performance DSP (esp-dsp), Edge Impulse C++ bundle integration, and advanced low-power ULP/RTC features—are more mature and easier to integrate using the ESP-IDF C++ framework.

Additionally, the project requires clear architectural documentation and code examples to facilitate future maintenance and extensions by local researchers and developers.

## What Changes

1.  **Architecture Migration**: Implement a C++ version of the ESP32-S3 firmware using the ESP-IDF framework.
2.  **Documentation First**: Create comprehensive documentation integrated into the codebase (via Markdown and Doxygen/Header comments) explaining the firmware architecture, power management cycles, and AI inference pipeline.
3.  **Code Examples**: Provide modular examples for key components: I2S audio capture, DSP filtering, TFLite Micro inference, and LoRa mesh communication.

## Capabilities

### New Capabilities
- `esp32-s3-cpp-firmware`: Implementation of the chainsaw detection firmware in C++, including optimized audio processing and low-power management.
- `firmware-architecture-docs`: Complete architectural documentation including timing diagrams, state machines, and integration guides.

### Modified Capabilities
- `lorawan-mesh-routing`: The C++ firmware will need to integrate with existing LoRa mesh specs for alert reporting.

## Impact

- **New Source Tree**: A new `src/esp32-cpp` (or similar) directory will be created.
- **Build System**: Addition of CMake-based build system for ESP-IDF C++.
- **Dependencies**: Integration of `esp-dsp`, `esp_nn`, and Edge Impulse C++ SDK.
- **Documentation**: New artifacts in `docs/firmware` or similar.
