## ADDED Requirements

### Requirement: Zero-Copy Ring Buffer Mechanism
The audio buffer system must utilize a shared memory ring buffer that allows DMA to write samples directly into the same memory space consumed by the DSP/AI logic.

#### Scenario: Shared Memory Access
- **WHEN** the I2S DMA controller completes a frame transfer.
- **THEN** it must write directly to the `SharedRingBuffer` memory pool without a CPU-driven `memcpy` operation.

### Requirement: Memory-Safe Concurrent Access
The ring buffer must manage read/write head pointers using atomic operations or semaphores to ensure thread-safety between the I2S ISR and the application processing task.

#### Scenario: Buffer Underflow/Overflow Prevention
- **WHEN** the processing task is slower or faster than the data capture rate.
- **THEN** the system must provide safe boundary signals (e.g., overflow flags) without corrupting memory or causing a crash.
