## Requirements

The `kernel-audio-capture` capability must provide a reliable mechanism to ingest audio samples from the BCM2837 I2S interface directly into kernel memory.

### Functional Requirements
- **I2S Configuration**: Must configure the I2S controller for 16kHz sampling rate, 32-bit sample width (mono).
- **DMA Integration**: Must use the Linux `dmaengine` API to manage data transfer from the I2S FIFO.
- **Continuous Capture**: Must support continuous data ingestion without dropping samples, using a circular buffer.
- **Resource Management**: Must properly request and release DMA channels and I2S registers.

### Performance Requirements
- **CPU Usage**: Data ingestion should consume < 2% CPU on a Raspberry Pi 3.
- **Latency**: Audio samples must be available in the ring buffer within 20ms of capture.

### Constraints
- Must run within the Linux Kernel (Rust-for-Linux environment).
- Specific to BCM2837 SoC (RPi 3).
