## ADDED Requirements

### Requirement: core-firmware-functionality
The firmware MUST perform real-time audio capture, spectral analysis, and AI inference to detect chainsaw sounds and biodiversity events (animals, insects, amphibians).

#### Scenario: positive-chainsaw-detection
- **WHEN** a chainsaw sound is detected (inference confidence > 0.8)
- **THEN** a LoRa alert message must be queued for transmission immediately.

#### Scenario: positive-biodiversity-detection
- **WHEN** a biodiversity event (animal, insect, or amphibian) is detected
- **THEN** the event must be logged, and a telemetry report should be queued for periodic transmission (according to the biodiversity reporting interval).

#### Scenario: spectral-pre-filter
- **WHEN** audio signal does not match known harmonic profiles (motor OR biological)
- **THEN** AI inference must be bypassed to save power.

### Requirement: low-power-management
The firmware MUST utilize ESP32-S3's power management features to maximize battery life.

#### Scenario: deep-sleep-cycle
- **WHEN** the system is in "Quiet" mode
- **THEN** the main CPUs must remain in deep sleep, with only the RTC/ULP monitoring RMS thresholds.
