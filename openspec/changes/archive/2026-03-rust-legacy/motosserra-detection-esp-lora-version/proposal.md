## Why

The current Guardian Edge AI node runs on a Raspberry Pi, which provides significant computational power for audio processing and machine learning inference. However, Raspberry Pi nodes are relatively expensive, consume significant power, and limit the scalability of a dense forest deployment. By migrating the detection logic to an ESP32 microcontroller with an integrated LoRa radio, we can create significantly cheaper, lower-power, and smaller sensor nodes. This enables a denser mesh network deployment, increasing the overall coverage and resilience of the chainsaw detection system in remote forest areas.

## What Changes

- **New Hardware Target**: Port the chainsaw detection application to target the ESP32 microcontroller architecture (specifically models with integrated LoRa like Heltec V3 or TTGO T-Beam).
- **Audio Capture Abstraction**: Implement an ESP32-compatible I2S audio capture module to read from digital MEMS microphones.
- **TFLite Micro Integration**: Adapt the `ai::DetectionEngine` to use TensorFlow Lite for Microcontrollers specifically optimized for the FreeRTOS ESP-IDF environment.
- **LoRa Driver Porting**: Adapt the existing `LoRaDriver` to use an ESP32-compatible LoRa library (e.g., RadioLib or LMIC) instead of the Linux/Raspberry Pi SPI drivers.
- **Firmware Build System**: Create a Rust `Cargo` build configuration utilizing the `esp-rs` ecosystem (e.g., `esp-idf-hal`) to compile the firmware for the ESP32 securely and efficiently.

## Capabilities

### New Capabilities
- `esp32-audio-capture`: I2S DMA-based audio capture for ESP32 MEMS microphones.
- `tflite-micro-esp32`: TensorFlow Lite Micro integration for ESP32 inference.
- `esp32-lora-driver`: Implementation of the LoRa physical layer for ESP32-integrated SX1262/SX1276 radios.
- `freertos-app-lifecycle`: Main application loop and power management targeting FreeRTOS.

### Modified Capabilities

## Impact

- **Code**: New ESP32-specific implementations will be written entirely in Rust. The system will bridge the core ML models and Mesh protocol logic.
- **Dependencies**: Requires the Rust toolchain with `esp-rs` tools (`cargo-espflash`), along with Rust crates for I2S audio and LoRa (e.g., `rust-lora` or bindings).
- **Systems**: The overall Guardian system architecture will expand to support heterogeneous nodes (Raspberry Pi Gateways/Routers and ESP32 Leaf nodes).
