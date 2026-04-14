## 1. Kernel DMA Infrastructure

- [x] 1.1 Request a dedicated DMA channel via the `dmaengine` API.
- [x] 1.2 Allocate a 64KB coherent DMA buffer and map it for both CPU and hardware access.
- [x] 1.3 Configure the DMA slave settings (DREQ channel, bus width, and direction).

## 2. Buffer Management & Synchronization

- [x] 2.1 Implement a cyclic DMA descriptor that loops back to the start of the buffer.
- [x] 2.2 Create a callback function to handle DMA "Transfer Complete" interrupts.
- [x] 2.3 Synchronize DMA buffer offsets with the user-space ring buffer sequence counters.

## 3. I2S Controller Integration

- [x] 3.1 Enable DMA Request (DREQ) signaling in the I2S `CS_A` and `MODE_A` registers.
- [x] 3.2 Implement logic to start DMA only when the first audio sample is requested.
- [x] 3.3 Ensure clean DMA channel release during module unloading.

## 4. Verification & Testing

- [x] 4.1 Verify DMA data integrity by comparing against known patterns in a simulated environment.
- [x] 4.2 Measure CPU utilization change during active 5s capture phases.
- [x] 4.3 Validate that `guardian_node` correctly reads from the DMA-updated shared memory.
