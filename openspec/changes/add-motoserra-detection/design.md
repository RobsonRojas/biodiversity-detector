## Context

The system needs to detect chainsaw sounds on a Raspberry Pi Zero (Cortex-A53, 512MB RAM) with extreme power constraints. We have a Rust-based driver providing audio at `/dev/forest_audio`.

## Goals / Non-Goals

**Goals:**
- Implement a 1-second sliding window buffer for audio processing.
- Integrate the TFLite Micro interpreter with INT8 quantization.
- Achieve a minimum confidence of 0.85 for detection.
- Maintain a target latency of 300ms per inference.

**Non-Goals:**
- Implementation of the LoRaWAN telemetry protocol (handled by another module).
- Training the AI model (assumed provided as a `.tflite` file).
- Implementation of the Rust kernel driver itself.

## Decisions

- **Buffer Strategy**: Use a circular buffer to store 16,000 samples (1s at 16kHz). This allows continuous processing without high memory overhead.
- **Quantization**: INT8 quantization is mandatory to meet the memory (512MB) and power (500mW) constraints of the RPi Zero.
- **Polling vs Interrupts**: The system will poll the device at the start of each duty cycle, process 5 seconds of audio, and then return to deep sleep.

## Risks / Trade-offs

- **Power vs. Latency**: Running inference consistently within 300ms on a single Cortex-A53 core might consume significant power. We must ensure the `duty_cycle` is strictly followed.
- **Model Size**: The TFLite Micro model must fit within the constrained LPDDR2 memory (512MB).
