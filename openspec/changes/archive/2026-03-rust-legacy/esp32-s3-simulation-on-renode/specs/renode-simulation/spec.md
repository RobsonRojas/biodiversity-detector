## ADDED Requirements

### Requirement: Boot ESP32-S3 Binary
The simulator SHALL successfully boot the compiled C++ firmware binary for the ESP32-S3 and reach the `setup()` or `guardian_init()` phase.

#### Scenario: Successful Boot
- **WHEN** the Renode `.resc` file points to a valid ELF/BIN firmware.
- **THEN** the UART log shows the initialization sequence of the Guardian core.

### Requirement: Serial Monitoring
The simulator SHALL forward UART output from the emulated ESP32-S3 to the host terminal or a log file.

#### Scenario: Log Verification
- **WHEN** the firmware prints an RMS value.
- **THEN** the RMS value appears in the host-side simulator console.
