## ADDED Requirements

### Requirement: Audio Sample Ingestion SHALL
The `kernel-audio-capture` capability SHALL provide a reliable mechanism to ingest audio samples from the BCM2837 I2S interface directly into kernel memory.

#### Scenario: Sample Capture
- **WHEN** the I2S hardware receives audio data.
- **THEN** the kernel driver SHALL transfer the data directly to a pre-allocated ring buffer.

### Requirement: I2S Hardware Configuration MUST
The system MUST configure the I2S controller for a 16kHz sampling rate and 32-bit sample width (mono).

#### Scenario: Controller Initialization
- **WHEN** the kernel module is loaded and the I2S peripheral is initialized.
- **THEN** the hardware registers MUST be set to 16kHz/32-bit mono mode.

### Requirement: DMA Engine Integration SHALL
The driver SHALL use the Linux `dmaengine` API to manage data transfers from the I2S FIFO to system memory.

#### Scenario: DMA Transfer
- **WHEN** the I2S FIFO reaches its threshold.
- **THEN** the DMA engine SHALL automatically move the data to the target kernel buffer.

### Requirement: Zero-Drop Capture MUST
The system MUST support continuous data ingestion without dropping samples, using a circular buffer architecture.

#### Scenario: Continuous Loop
- **WHEN** the capture process runs for an extended period.
- **THEN** the circular buffer MUST wrap without loss of audio continuity.

### Requirement: Latency Constraints SHALL
Audio samples SHALL be available in the ring buffer within 20ms of capture.

#### Scenario: Real-time Processing
- **WHEN** a sound event occurs.
- **THEN** the timestamp in the kernel buffer SHALL reflect the event time with less than 20ms error.
