## Why

The current audio capture implementation uses standard ESP-IDF drivers which may introduce jitter or latency unsuitable for high-fidelity real-time AI inference. By implementing a direct register-level I2S interface and a zero-copy shared memory ring buffer, we can achieve significantly lower latency, improved memory safety, and more reliable real-time access to audio data, which is critical for accurate chainsaw detection in complex environments.

## What Changes

- Implementation of a dedicated I2S driver interacting directly with the ESP32-S3 SoC registers.
- Introduction of a Shared Memory Ring Buffer for zero-copy data transfer between hardware interrupts and the DSP/AI pipeline.
- Optimization of hardware interrupt handling to minimize CPU wake-up latency and prevent buffer overruns.

## Capabilities

### New Capabilities
- `i2s-high-fidelity-capture`: High-fidelity, low-latency audio capture from I2S microphones using direct register access.
- `zero-copy-audio-transfer`: Real-time, memory-safe data transfer mechanism using a shared ring buffer.

### Modified Capabilities
- `audio-capture`: Improving the underlying implementation to support real-time requirements.

## Impact

- `src/esp32-s3-cpp/main/audio_capture.cpp`: Refactoring to use the new low-level I2S driver.
- `src/esp32-s3-cpp/main/audio_buffer.cpp`: Refactoring to implement the zero-copy ring buffer mechanism.
- `ESP-IDF Drivers`: Bypassing higher-level abstractions for I2S where performance is critical.
