## ADDED Requirements

### Requirement: Multi-core Task Allocation
The system SHALL utilize Rust `std::thread` (backed by ESP-IDF FreeRTOS) to allocate time-critical tasks appropriately across the ESP32 dual cores.

#### Scenario: System boot
- **WHEN** the system boots
- **THEN** the audio capture thread is pinned to Core 0 and the ML inference thread is pinned to Core 1

### Requirement: Sleep Scheduling
The system SHALL implement sleep phases to conserve battery life when not actively detecting or transmitting.

#### Scenario: Periodic sleep
- **WHEN** the 5-second active phase concludes without a detection
- **THEN** the system enters a light or deep sleep state using Rust sleep APIs for 55 seconds
