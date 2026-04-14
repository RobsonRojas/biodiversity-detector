## Why

The current interrupt-driven audio capture mechanism (I2S FIFO to memory) consumes significant CPU cycles and is susceptible to data loss under high system load. Implementing a Direct Memory Access (DMA) interface will offload the data transfer task from the CPU, ensuring stable, zero-copy, and low-latency audio ingestion even at high sample rates (16kHz+ @ 32-bit).

## What Changes

We will introduce a kernel-level DMA engine interface specifically for the I2S peripheral. This involves:
1. Configuring the DMA controller to handle I2S RX requests.
2. Synchronizing the DMA-transferred data with the existing Shared-Memory Ring Buffer.
3. Updating the C++ `guardian_node` to handle DMA-backed buffer updates.

## Capabilities

### New Capabilities
- `dma-audio-transfer`: Implements Direct Memory Access for the I2S audio stream, bypassing the CPU for data movement between the peripheral and the ring buffer.

### Modified Capabilities
- `kernel-audio-driver`: Updated to include DMA configuration and synchronization logic alongside the existing character device infrastructure.

## Impact

- **Kernel Module**: Significant changes to include DMA channel allocation and interrupt handling for DMA events.
- **Hardware**: Tight coupling with the Raspberry Pi (SoC) DMA controller registers.
- **Performance**: Reduced CPU usage during audio capture, improved system responsiveness.
