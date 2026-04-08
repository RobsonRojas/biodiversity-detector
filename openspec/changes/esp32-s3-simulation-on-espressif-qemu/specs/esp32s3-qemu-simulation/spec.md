## ADDED Requirements

### Requirement: Espressif-QEMU-Simulation-Environment
The system SHALL provide a Dockerized simulation environment using the official Espressif QEMU fork (`qemu-system-xtensa`) to emulate ESP32-S3 hardware.

#### Scenario: Successful QEMU Startup
- **WHEN** the simulation runner starts an ESP32-S3 node
- **THEN** a QEMU instance is spawned with the correct bootloader, partition table, and firmware binary

### Requirement: QEMU-Socket-UART-Bridge
The system SHALL establish a socket-based bridge (TCP/UDP) for the QEMU guest UART to enable communication with host-side simulation utilities.

#### Scenario: Bidirectional UART Communication
- **WHEN** the QEMU guest writes to the UART console
- **THEN** the simulation runner captures the output and forwards it to the MeshRelay socket on the host

### Requirement: Offline-Build-Binary-Loading
The system SHALL support loading pre-compiled binary artifacts into QEMU without requiring internet access for toolchain installation during runtime.

#### Scenario: Pre-compiled Image Loading
- **WHEN** valid firmware images exist in the `dist/` directory
- **THEN** QEMU loads these images using physical memory mapping or the `-drive` interface as per Espressif QEMU standards
