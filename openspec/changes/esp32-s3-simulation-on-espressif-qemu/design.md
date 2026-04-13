## Context

The GuardianCore project requires a high-fidelity simulation of the ESP32-S3 node for mesh network validation. Renode proved insufficient due to lack of Xtensa support in the local environment and Docker integration complexities. Espressif's fork of QEMU (`qemu-system-xtensa`) is designed specifically for this architecture and supports the necessary bootloader and peripheral models to run the official firmware.

## Goals / Non-Goals

**Goals:**
- Provide a robust ESP32-S3 simulation using Espressif QEMU.
- Enable a 5-node heterogeneous simulation (Native Linux nodes + QEMU nodes).
- Support binary loading (bootloader, partition table, app) from the `dist/` directory.
- Expose guest UART via TCP/UDP sockets for MeshRelay integration.
- Generate animated map video from simulation logs showing node topology, mesh traffic, and detection events.

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

### 4. Virtual LoRa Bridge (UART)
- **Decision**: Implement a thin protocol over UART to simulate LoRa packet ingestion.
- **Protocol**: `LORA_RX:BASE64_DATA:RSSI\n` and `LORA_TX:BASE64_DATA\n`.
- **Rationale**: QEMU does not emulate the SX1262 LoRa chip. Standardizing on a UART-based bridge allows the firmware to receive geolocation beacons and telemetry from the simulated mesh net without custom hardware models.

### 3. Unified Image Deployment
- **Decision**: Utilize a Python-based `qemu_runner.py` to orchestrate booting with separate binary components (bootloader, partitions, app).
- **Rationale**: ESP-IDF binaries are typically split. Loading them individually via `-drive if=pflash` or `-device loader` is cleaner than manually merging into a 4MB/8MB blob every time.

### 5. Visual Simulation Map Output (`sim_visualizer.py`)
- **Decision**: Use Matplotlib + FFMpeg to generate a cinematic animated map from post-simulation log files.
- **Rationale**: Matplotlib is already available in the environment and provides the compositing flexibility needed for a multi-layer map (terrain, nodes, traffic, HUD). FFMpeg handles encoding to MP4. No additional dependencies needed.
- **Architecture**:
  - **Log Parser**: Reads `sim_logs/nodeN.log` files and reconstructs a timeline of events (heartbeats, relays, alerts, detections).
  - **Map Renderer**: Plots nodes on a lat/lon grid with a stylized dark forest background, contour lines, and LoRa range circles.
  - **Animation Engine**: Steps through the event timeline, animating packet flow between nodes and detection pulses.
  - **HUD Dashboard**: Side panel showing per-node battery, RSSI, and detection counts, updated per frame.
  - **Output**: 1920×1080 MP4 at 30 FPS saved to `sim_output/musa_simulation.mp4`.
- **Alternative**: Using a web-based renderer (Leaflet.js + Puppeteer recording) — rejected for complexity and additional dependencies.

## Risks / Trade-offs

- **[Risk] Resource Overload** → QEMU can be CPU intensive.  
  *Mitigation*: Limit the number of QEMU nodes to what's strictly necessary for validating the mesh protocols (e.g., 1 QEMU node + 4 Native nodes).
- **[Risk] Complex Toolchain** → Building Espressif QEMU from source is slow.  
  *Mitigation*: Use a pre-built Docker image or binary distribution of Espressif tools (esp-idf container already includes it sometimes).
- **[Risk] Low-Fidelity Propagation** → UART is slower than SPI.  
  *Mitigation*: Use a high baud rate (921600) for the virtual UART and ensure the firmware's polling loop is optimized for simulation.
