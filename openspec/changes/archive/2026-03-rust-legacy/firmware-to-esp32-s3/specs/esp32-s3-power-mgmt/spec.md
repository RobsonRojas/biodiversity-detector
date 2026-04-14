## ADDED Requirements

### Requirement: Wake-on-Sound Interrupt
The firmware MUST utilize the ESP32-S3's Low Power GPIO or ULP-FSM to monitor the wake-up signal from the microphone during Deep Sleep.

#### Scenario: Deep Sleep Entry
- **WHEN** the AI inference completes and no active chainsaw is detected.
- **THEN** the system must enter Deep Sleep, disabling all peripherals except the I2S-Wake or GPIO-Wake source.

#### Scenario: RTC State Persistence
- **WHEN** the system wakes from sleep to perform inference.
- **THEN** it must preserve the timestamp and the peak RMS value of the triggering event in RTC recovery memory.

### Requirement: Selective Peripheral Powering
The system MUST manage power to the microphone and external storage (e.g., SD Card) independently to maximize battery life.

#### Scenario: Model Loading
- **WHEN** the AI stage starts.
- **THEN** the system must power up the flash/storage only if the TinyML model weight buffer is not already in SRAM.
