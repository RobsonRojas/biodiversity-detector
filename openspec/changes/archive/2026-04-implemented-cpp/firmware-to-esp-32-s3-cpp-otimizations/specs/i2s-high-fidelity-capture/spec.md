## ADDED Requirements

### Requirement: Register-Level I2S Initialization
The system MUST initialize the I2S0 peripheral by directly writing to the SoC configuration registers to ensure optimal timing and clock stability.

#### Scenario: Successful I2S0 Low-Level Start
- **WHEN** the `I2SDriver::init()` method is called.
- **THEN** it must bypass standard drivers and configure the S3 I2S registers for high-fidelity 44.1kHz capture.

### Requirement: Sub-millisecond Capture Latency
The driver MUST propagate audio data from the hardware to the ring buffer with a latency contribution of less than 1ms.

#### Scenario: Real-time Audio Stream Access
- **WHEN** the 44.1kHz audio stream is active.
- **THEN** the samples must be available in the shared buffer immediately following DMA transfer completion.
