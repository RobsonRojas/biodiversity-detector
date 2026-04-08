## ADDED Requirements

### Requirement: I2S Audio Feeding
The emulation SHALL support feeding raw 16-bit PCM audio samples into the emulated application's I2S capture buffers.

#### Scenario: Audio Sample Injection
- **WHEN** a `.wav` file is provided as a Source to the Renode script.
- **THEN** the emulated firmware receives the corresponding samples in its DMA buffer.

### Requirement: RTC Sleep Emulation
The emulation SHALL correctly enter and exiting low-power states when the firmware calls `esp_deep_sleep_start()`.

#### Scenario: Wake-on-Sound Simulation
- **WHEN** the firmware enters deep sleep.
- **THEN** the simulation should pause or enter a low-tick mode until a simulated "sound wake-up" event is triggered.
