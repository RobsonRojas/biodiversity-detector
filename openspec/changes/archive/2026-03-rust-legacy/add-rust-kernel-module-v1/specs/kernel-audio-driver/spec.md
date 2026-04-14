# Capability: kernel-audio-driver

## ADDED Requirements

### Requirement: character-device-node
The module MUST register a character device at `/dev/forest_audio` with read-only permissions for the `guardian` group.

#### Scenario: device-registration
- **WHEN** the kernel module is loaded via `insmod`
- **THEN** the character device node `/dev/forest_audio` is created and accessible.

### Requirement: mmap-zero-copy
The driver MUST support the `mmap` system call to allow user-space processes to map the kernel ring buffer into their address space.

#### Scenario: shared-memory-access
- **WHEN** a user-space process calls `mmap` on the `/dev/forest_audio` file descriptor
- **THEN** it gains zero-copy access to the audio sample stream.

### Requirement: host-simulation-mode
The `guardian_node` MUST support a command-line argument to specify a custom device path, enabling simulation on non-ARM hosts using regular files.

#### Scenario: simulation-execution
- **WHEN** `guardian_node /tmp/mock_audio` is executed
- **THEN** it opens `/tmp/mock_audio` instead of the default `/dev/forest_audio` and processes its content as audio data.
