## ADDED Requirements

### Requirement: core-firmware-functionality
The firmware MUST perform real-time audio capture, spectral analysis, and AI inference to detect chainsaw sounds.

#### Scenario: positive-detection
- **WHEN** a chainsaw sound is detected (inference confidence > 0.8)
- **THEN** a LoRa alert message must be queued for transmission immediately.

#### Scenario: spectral-pre-filter
- **WHEN** audio signal does not match the harmonic profile of a motor (e.g., wind noise)
- **THEN** AI inference must be bypassed to save power.

### Requirement: low-power-management
The firmware MUST utilize ESP32-S3's power management features to maximize battery life.

#### Scenario: deep-sleep-cycle
- **WHEN** the system is in "Quiet" mode
- **THEN** the main CPUs must remain in deep sleep, with only the RTC/ULP monitoring RMS thresholds.
