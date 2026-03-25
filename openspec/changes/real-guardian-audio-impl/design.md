## Context

The `guardian_audio` module is a Rust-based kernel driver for the Raspberry Pi 3. It currently lacks a functional implementation for I2S and DMA. This design outlines the transition from placeholders to real hardware interaction using the `dmaengine` subsystem and direct register access for I2S.

## Goals / Non-Goals

### Goals:
- Implement `configure_i2s` to set up BCM2837 I2S for 16kHz/32-bit mono.
- Implement `start_dma` and `stop_dma` using the Linux `dmaengine` API.
- Use `dma_alloc_coherent` for a 64KB circular buffer.
- Implement `dma_callback` to handle half-buffer and full-buffer completion.

### Non-Goals:
- Supporting multiple sample rates (hardcoded to 16kHz for motoserra detection).
- Implementing ALSA/ASoC framework (custom character device is preferred for low-latency/simplicity).

## Decisions

- **Direct I2S Register Access**: We will use `ioremap` to access I2S registers (`CS_A`, `MODE_A`, etc.) because the standard ASoC driver might be too heavy or unavailable in the target environment.
- **DMA Engine API**: We will use the standard Linux `dmaengine` API instead of manual DMA register manipulation to ensure better compatibility with the kernel's DMA controller management.
- **Circular Buffer**: A 64KB buffer (covering ~1 second of 16kHz/32-bit audio) will be split into two 32KB halves for double buffering.

## Risks / Trade-offs

- **DMA Conflict**: Other drivers (like PWM or SPI) might be using DMA channels. Mitigation: Use `dma_request_chan` to let the kernel manage channel allocation.
- **Cache Coherency**: Accessing DMA buffers from both CPU and hardware can lead to staleness. Mitigation: Use `dma_alloc_coherent`.
- **Interrupt Latency**: High system load might delay the `dma_callback`. Mitigation: Use a large enough buffer to tolerate small jitters.
