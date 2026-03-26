## 1. Project Initialization

- [x] 1.1 Set up Rust `cargo` build environment configuring `.cargo/config.toml` for `xtensa` or `riscv` ESP32 targets.
- [x] 1.2 Add `esp-idf-hal` and `esp-idf-sys` crate dependencies.

## 2. Hardware Drivers (Rust)

- [x] 2.1 Implement I2S audio capture using `esp-idf-hal::i2s` DMA API.
- [x] 2.2 Implement LoRa transmission/reception using the `sx126x` or equivalent Rust crate.

## 3. Machine Learning Port

- [x] 3.1 Integrate `tflite-micro-rs` (or generate C bindings for TFLite Micro) into the Rust project.
- [x] 3.2 Adapt `DetectionEngine` struct to use Rust-based TFLite Micro invocation.
- [x] 3.3 Set up static `[u8; N]` tensor arena memory block in Rust `static` memory.

## 4. System Integration & Lifecycle

- [x] 4.1 Implement main boot loop using standard Rust `main()` mapped to `esp-idf` startup.
- [x] 4.2 Spawn Rust `std::thread` for continuous Audio Capture (pinned to Core 0).
- [x] 4.3 Spawn Rust `std::thread` for ML Inference and LoRa transmission (pinned to Core 1).
- [x] 4.4 Implement deep sleep scheduling logic using `esp-idf-hal::delay` and sleep APIs.

## 5. Verification

- [x] 5.1 Flash firmware to ESP32 test node.
- [x] 5.2 Verify I2S microphone captures valid audio data.
- [x] 5.3 Verify TFLite model successfully executes and outputs confidences.
- [x] 5.4 Verify ESP32 node successfully sends MeshPackets to a Raspberry Pi Gateway node or test receiver.
