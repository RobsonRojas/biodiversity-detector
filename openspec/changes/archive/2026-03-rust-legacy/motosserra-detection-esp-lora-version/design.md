## Context

The Guardian Edge AI system currently monitors chainsaw noises using a Raspberry Pi. While effective, the Raspberry Pi is relatively power-hungry and expensive for a densely deployed mesh sensor network in a forest environment. The ESP32 offers a low-cost, low-power alternative with integrated LoRa support, ideal for acting as Leaf nodes in the forest.

## Goals / Non-Goals

**Goals:**
- Port the audio capture and ML inference logic to run on an ESP32 (e.g., Heltec V3).
- Implement an I2S DMA-based audio capture system optimized for ESP32.
- Integrate TensorFlow Lite for Microcontrollers (TFLite Micro) for inference.
- Use a LoRa library to communicate over the existing AU915 LoRaWAN mesh network.
- Ensure the system can run on a battery/solar setup by utilizing FreeRTOS tasks and deep sleep capabilities.

**Non-Goals:**
- Dropping support for the Raspberry Pi nodes (they will act as Routers/Gateways).
- Retraining the ML model (we assume the current model can be quantized/converted for TFLite Micro).
- Implementing a dynamic routing protocol (we stick to the static routing table).

## Decisions

- **Build System**: Use Rust with the `esp-idf-hal` framework (`std` approach). Rationale: Using Rust provides memory safety and fearless concurrency. `esp-idf-hal` allows leveraging existing ESP-IDF drivers (like I2S DMA) while writing the application logic safely in Rust.
- **Audio Interface**: Use the Rust I2S bindings (`esp-idf-hal::i2s`) to interface with an INMP441 I2S MEMS microphone. Rationale: Digital I2S microphones provide cleaner audio, and DMA seamless buffering is well-supported in the ESP Rust HAL.
- **Inference Engine**: Use `tflite-micro-rs` or C-bindings to TensorFlow Lite for Microcontrollers. Rationale: It's the standard for running quantized ML models.
- **LoRa Stack**: Use a Rust LoRa crate (e.g., `lora-rs` or `sx126x` driver). Rationale: A pure Rust driver avoids unsafe C bindings where possible, allowing us to implement our custom mesh protocol reliably.

## Risks / Trade-offs

- [Risk: Memory Constraints] → TFLite Micro requires a pre-allocated tensor arena. We must optimize the model (e.g., int8 quantization) and carefully manage FreeRTOS task stack sizes to ensure the ESP32's SRAM (typically ~520KB) is not exhausted.
- [Risk: Inference Time] → The ESP32 is much slower than a Raspberry Pi. → We will leverage the ESP32's dual-core architecture by dedicating one core to audio capture/network and the other to ML inference.
