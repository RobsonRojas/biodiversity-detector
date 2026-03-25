## Why

The current `guardian_audio` kernel module uses placeholder implementations with `pr_info` logs instead of actual hardware interaction. To enable real-time motoserra detection, the module must be capable of high-performance audio capture from the I2S interface using DMA to minimize CPU overhead on the Raspberry Pi 3 (BCM2837).

## What Changes

- Implement real I2S controller configuration for 16kHz/32-bit MONO audio capture.
- Integrate with the Linux `dmaengine` API to request and configure a DMA channel for I2S RX.
- Implement a circular DMA buffer with half-transfer and full-transfer interrupt handling for continuous capture.
- Replace placeholder logic in `open`, `read`, `release`, and `mmap` with real hardware and buffer synchronization logic.

## Capabilities

### New Capabilities
- `kernel-audio-capture`: High-performance, low-latency audio ingestion from I2S to kernel-space ring buffer via DMA.
- `dma-buffer-streaming`: Efficient synchronization of DMA-filled buffers with user-space via `mmap` and sequence counters.

### Modified Capabilities
- None

## Impact

- `src/kernel/module_guardian_audio.rs`: Significant changes to implement hardware-specific logic.
- Kernel build configuration: May require specific DMA and I2S driver support in the kernel.
- User-space applications: Will now receive real audio data instead of placeholders.
