## Context

The GuardianCore project requires a high-fidelity simulation of the ESP32-S3 node for mesh network validation. Renode proved insufficient due to lack of Xtensa support in the local environment and Docker integration complexities. Espressif's fork of QEMU (`qemu-system-xtensa`) is designed specifically for this architecture and supports the necessary bootloader and peripheral models to run the official firmware.

## Goals / Non-Goals

**Goals:**
- Provide a robust ESP32-S3 simulation using Espressif QEMU.
- Enable a 5-node heterogeneous simulation (Native Linux nodes + QEMU nodes).
- Support binary loading (bootloader, partition table, app) from the `dist/` directory.
- Expose guest UART via TCP/UDP sockets for MeshRelay integration.

**Non-Goals:**
- Full hardware peripheral emulation (e.g., I2S, ADC) beyond what's needed for the UART bridge and boot sequence.
- Real-time Wi-Fi/Bluetooth emulation (stubs are sufficient).

## Decisions

### 1. Espressif QEMU Fork (`qemu-system-xtensa`)
- **Decision**: Standardize on the official Espressif QEMU fork.
- **Rationale**: It provides the most accurate Xtensa LX7 core model and supports ESP-IDF specific features like the bootloader's expectations for flash mapping.
- **Alternatives**: Using a generic Xtensa QEMU or Renode (already ruled out).

### 2. Socket-based Serial Bridge
- **Decision**: Use QEMU's `-serial chardev:id` connected to a TCP server socket.
- **Rationale**: This matches the existing MeshRelay pattern where nodes are accessible via TCP/UDP ports.
- **Alternative**: Using PTYs (harder to manage in Docker) or log files (unidirectional).

### 3. Unified Image Deployment
- **Decision**: Utilize a Python-based `qemu_runner.py` to orchestrate booting with separate binary components (bootloader, partitions, app).
- **Rationale**: ESP-IDF binaries are typically split. Loading them individually via `-drive if=pflash` or `-device loader` is cleaner than manually merging into a 4MB/8MB blob every time.

## Risks / Trade-offs

- **[Risk] Resource Overload** → QEMU can be CPU intensive.  
  *Mitigation*: Limit the number of QEMU nodes to what's strictly necessary for validating the mesh protocols (e.g., 1 QEMU node + 4 Native nodes).
- **[Risk] Complex Toolchain** → Building Espressif QEMU from source is slow.  
  *Mitigation*: Use a pre-built Docker image or binary distribution of Espressif tools (esp-idf container already includes it sometimes).
