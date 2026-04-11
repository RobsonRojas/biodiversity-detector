## 1. Low-Level I2S Driver Implementation

- [ ] 1.1 Implement register-level I2S0 configuration using `hal/i2s_ll.h`.
- [ ] 1.2 Configure I2S clock and data format for 44.1kHz, 32-bit resolution.
- [ ] 1.3 Setup DMA descriptors pointing to the `SharedRingBuffer` segments.
- [ ] 1.4 Implement a high-priority ISR for I2S DMA EOF interrupts.

## 2. Shared Memory Ring Buffer

- [ ] 2.1 Develop the `SharedRingBuffer` class for circular audio sample management.
- [ ] 2.2 Implement zero-copy accessors for the DSP and AI pipeline tasks.
- [ ] 2.3 Integrate FreeRTOS semaphores for thread-safe head/tail pointer synchronization between the ISR and consumer tasks.

## 3. Integration and Optimization

- [ ] 3.1 Refactor the `AudioCapture` component to wrap the new low-level driver logic.
- [ ] 3.2 Update `app_main` to utilize the zero-copy buffer stream for RMS monitoring and AI inference.
- [ ] 3.3 Conduct latency profiling to verify sub-millisecond hardware-to-buffer propagation.
- [ ] 3.4 Validate memory safety and stability under high CPU load conditions.
