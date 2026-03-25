## Context

The current `guardian_audio` kernel module reads samples from the I2S FIFO in an interrupt handler. While functional, this approach requires the CPU to wake up for every few samples, leading to high context-switch overhead and potential jitter. Transitioning to a DMA-based approach allows the SoC's DMA controller to handle the transfer of large blocks of samples directly into a pre-allocated memory region.

## Goals / Non-Goals

**Goals:**
- Offload sample transfer from I2S FIFO to memory using DMA.
- support a circular DMA buffer that aligns with the user-space accessible ring buffer.
- Achieve < 1% CPU utilization for the data ingestion phase.

**Non-Goals:**
- Implementing a generic DMA framework (this is specific to I2S).
- Supporting older RPi models without the BCM2837/BCM2835 DMA controller.
- Managing DMA for the LoRa/SPI interface.

## Decisions

- **DMA Controller**: Use the Broadcom DMA Engine (Legacy DMA) for its deterministic behavior with I2S.
- **Memory Allocation**: Use `dma_alloc_coherent` to ensure the buffer is accessible by both the DMA controller and the CPU without cache coherency issues.
- **Signal Handling**: Use the I2S DREQ (Data Request) signal to trigger DMA transfers automatically when the FIFO reaches a specific threshold.
- **Interrupts**: Trigger a DMA interrupt only when a half-buffer or full-buffer transfer is complete to inform the synchronization layer.

## Risks / Trade-offs

- **Memory Fragmentation**: `dma_alloc_coherent` requires contiguous physical memory, which might fail if the system has been running for a long time (though unlikely on this dedicated sensor node).
- **Complexity**: DMA debugging is notoriously difficult compared to simple PIO (Programmed I/O).
- **Hard-coded Channels**: We may need to reserve a specific DMA channel (e.g., Channel 4 or 5) which could conflict with other system drivers if not carefully managed.
