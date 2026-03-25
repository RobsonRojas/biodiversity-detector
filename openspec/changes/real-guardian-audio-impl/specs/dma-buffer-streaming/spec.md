## Requirements

The `dma-buffer-streaming` capability handles the synchronization of the DMA circular buffer with user-space consumers.

### Functional Requirements
- **Wait/Notify Mechanism**: Provide a way for user-space to wait for new data (e.g., via `poll` or sequence counters).
- **Safe Memory Mapping**: Support `mmap` to allow user-space to read directly from the DMA-coherent buffer.
- **Sequence Counters**: Maintain atomic sequence counters representing the current write offset of the DMA engine.
- **Half-Buffer Interrupts**: Trigger notifications/updates when the DMA reaches the half-way point and the end of the circular buffer.

### Security Requirements
- **Read-Only Access**: User-space should have read-only access to the audio buffer to prevent corruption of the capture process.
- **Proper Cleanup**: Ensure all mappings are invalidated when the module is removed.
