## Why

The previous attempt to use Renode for ESP32-S3 simulation was blocked by the lack of Xtensa architecture support in the current environment and persistent Docker networking issues during firmware cross-compilation. Switching to Espressif's official QEMU fork provides a more robust and supported path for simulating ESP32-S3 firmware, enabling rigorous validation of the multi-hop LoRa mesh network and end-to-end integration with the Web Manager without requiring internet access for build tools once the Docker image is prepared.

## What Changes

- **Simulator Engine**: Replace Renode with Espressif QEMU (`qemu-system-xtensa`) for simulating the ESP32-S3 leaf node.
- **Docker Integration**: Update the simulation Docker environment to include the Espressif QEMU binary and its dependencies.
- **Bootloader & Partitioning**: Configure QEMU to correctly handle the ESP32-S3 bootloader, partition table, and app binary loading.
- **UART Bridge**: Implement a socket-based UART bridge for QEMU to connect with the MeshRelay and other simulated nodes.
- **Audio Mocking**: Adapt the audio injection strategy to work with QEMU's peripheral emulation or memory injection capabilities.
- **Geolocation Propagation**: Validate the mesh's coordinate convergence logic using emulated neighbor coordinates injected via the UART bridge.
- **Gateway Simulation**: Implement a dedicated "Gateway Mode" in the simulation bridge to broadcast authoritative geographic reference points (lat/lon) to the mesh, verifying that leaf nodes accurately converge on their own coordinates through multi-lateration.
- **Visual Map Output**: Generate an animated map visualization of the simulation showing node positions, mesh traffic, detection events, and telemetry. Output saved as MP4 video.

## Capabilities

### New Capabilities
- `esp32s3-qemu-simulation`: Defines the requirements for running ESP32-S3 firmware in a QEMU-based environment, including UART connectivity and memory-mapped peripheral simulation.
- `visual-simulation-map-output`: Generates an animated map video from simulation logs, rendering node positions over a stylized forest background with animated mesh traffic, detection alerts, and a live telemetry dashboard.

### Modified Capabilities
- `lorawan-mesh-routing`: Update requirements to include validation via QEMU-simulated leaf nodes and geolocation propagation.

## Impact

- **Build System**: Requires generating valid ESP32-S3 binaries including bootloader and partition table.
- **CI/CD**: Simulation tests will now run inside QEMU-enabled Docker containers.
- **Simulation Scripts**: `renode_runner.py` will be replaced or supplemented by `qemu_runner.py`.
- **Docker Images**: `Dockerfile.sim` will be updated to include Espressif QEMU.
