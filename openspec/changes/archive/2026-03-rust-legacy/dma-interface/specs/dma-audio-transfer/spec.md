# Capability: dma-audio-transfer

## ADDED Requirements

### Requirement: dma-engine-configuration
The kernel module MUST request and configure a DMA channel from the BCM2835 DMA engine, mapping it to the I2S peripheral's Data Request (DREQ) signal.

#### Scenario: dma-initialization
- **WHEN** the `guardian_audio` module is initialized
- **THEN** it successfully allocates a DMA channel and links it to the I2S RX DREQ.

### Requirement: circular-dma-buffer
The driver MUST allocate a contiguous physical memory buffer (via `dma_alloc_coherent`) and configure the DMA engine to work in a circular/cyclic mode.

#### Scenario: buffer-allocation
- **WHEN** the capture starts
- **THEN** a 64KB buffer is allocated and the DMA control blocks are programmed to loop automatically.

### Requirement: dma-interrupt-handling
The driver MUST implement an interrupt handler for the DMA controller that triggers on "Transfer Complete" and "Half-Transfer Complete" events.

#### Scenario: data-synchronization
- **WHEN** the DMA controller finishes filling half of the circular buffer
- **THEN** an interrupt is fired, and the driver updates the head pointer of the shared-memory ring buffer for `guardian_node` to read.
