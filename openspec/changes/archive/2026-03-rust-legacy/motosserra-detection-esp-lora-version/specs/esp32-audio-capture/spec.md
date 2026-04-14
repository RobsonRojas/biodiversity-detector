## ADDED Requirements

### Requirement: I2S DMA Audio Capture
The system SHALL capture environmental audio using the ESP32 I2S peripheral with DMA support.

#### Scenario: Continuous recording
- **WHEN** the system enters the active listening phase
- **THEN** it must continuously fill the audio circular buffer without CPU blocking using DMA transfers

### Requirement: Sample Rate Consistency
The system SHALL capture audio at exactly 16 kHz to match the ML model's expected input.

#### Scenario: Verify sample rate
- **WHEN** audio is passed to the detection engine
- **THEN** exactly 16000 samples must correspond to 1 second of real-world audio
