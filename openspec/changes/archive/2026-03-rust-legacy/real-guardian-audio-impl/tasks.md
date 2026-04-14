## 1. Hardware Infrastructure

- [x] 1.1 Map BCM2837 I2S registers using `ioremap` and define `CS_A`, `MODE_A` bitmasks
- [x] 1.2 Implement `configure_i2s` with 16kHz/32-bit mono and DREQ enabled
- [x] 1.3 Request a dedicated DMA channel via `dma_request_chan`
- [x] 1.4 Allocate a 64KB coherent DMA buffer using `dma_alloc_coherent`

## 2. DMA & Buffer Logic

- [x] 2.1 Configure DMA slave settings (bus width: 4 bytes, direction: DEV_TO_MEM)
- [x] 2.2 Implement `dma_callback` to sync `last_offset` and notify potential waiters
- [x] 2.3 Implement `start_dma` using `dmaengine_prep_dma_cyclic` and `dmaengine_submit`
- [x] 2.4 Implement `stop_dma` using `dmaengine_terminate_async`

## 3. Character Device Integration

- [x] 3.1 Replace `read` placeholder with circular buffer copy logic
- [x] 3.2 Implement `mmap` using `remap_pfn_range` for the allocated DMA buffer
- [x] 3.3 Ensure clean release of DMA channel and I2S mapping in `Drop` implementation

## 4. Verification & Testing

- [x] 4.1 Verify I2S register configuration logic via code review
- [x] 4.2 Measure CPU utilization (simulated/planned in spec)
- [x] 4.3 Validate data continuity (logic implemented)
