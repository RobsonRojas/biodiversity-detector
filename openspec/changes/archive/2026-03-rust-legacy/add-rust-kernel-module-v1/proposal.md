## Why

The Guardian system requires high-fidelity, low-latency audio capture from I2S microphones to detect illegal logging activities. A traditional user-space approach may suffer from jitter and scheduling delays. Implementing a Rust-based kernel module ensures reliable, real-time access to the hardware, improved memory safety over C, and better integration with our optimized C++ detection engine.

## What Changes

1.  **Kernel-to-User Interface**: Implementation of `/dev/forest_audio`, a character device node.
2.  **Shared Memory Ring Buffer**: Zero-copy transfer mechanism for audio samples.
3.  **I2S Controller Integration**: Direct register-level interface with the SoC I2S controller.
4.  **Interrupt Handling**: Efficient management of hardware interrupts to fill the ring buffer.

## Capabilities

### New Capabilities
- `kernel-audio-driver`: Low-level access to acoustic sensors with real-time stability and memory safety.
- `i2s-hardware-interface`: Direct management of the I2S hardware registers and DMA controllers.

### Modified Capabilities
- (None)

## Impact

- **Hardware Layer**: Direct interaction with the SoC (aarch64) memory mapping.
- **System Stability**: Kernel-level implementation; errors will result in kernel panics.
- **Build System**: Requires kernel headers and a Rust-for-Linux toolchain.
