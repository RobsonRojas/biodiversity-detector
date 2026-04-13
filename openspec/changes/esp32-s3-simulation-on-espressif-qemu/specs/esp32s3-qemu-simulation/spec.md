## ADDED Requirements

### Requirement: Geolocation-Propagation-Simulation
The system SHALL simulate the propagation of geographic coordinates across the mesh using RSSI values derived from simulated distances between nodes.

#### Scenario: Mesh Coordinate Convergence
- **WHEN** the gateway anchor emits beacons with its precise GPS coordinates
- **THEN** neighbor nodes derive their relative distance via simulated RSSI and eventually calculate their own coordinates via multi-lateration
- **AND** leaf nodes multiple hops away eventually receive and converge on a position with estimated error <= 50m

### Requirement: Acoustic-Source-Localization-Simulation
The system SHALL simulate the localization of sound sources by correlating detections from multiple simulated nodes.

#### Scenario: Sound Source Triangulation
- **WHEN** a sound event is "emitted" at a specific virtual (lat, lon) in the simulation environment
- **THEN** all nodes within the simulated acoustic range record a detection with confidence weighted by distance
- **AND** the system (Gateway or Cloud) correctly calculates the sound source centroid within a 30m radius of the emitted point

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

### Requirement: Visual-Simulation-Map-Output
The system SHALL produce a visual map representation of the simulation, rendering all mesh nodes at their geographic coordinates and animating network events in real time. The output SHALL be saved as a video file (MP4).

#### Scenario: Animated Map Generation from Logs
- **WHEN** the simulation completes and per-node log files exist in `sim_logs/`
- **THEN** a Python-based visualizer parses the logs and generates a map animation showing:
  - Node positions plotted on a geographic coordinate grid
  - Node roles distinguished by icon shape/color (Leaf, Router, Gateway)
  - Mesh relay paths animated as moving packets between nodes
  - Detection alerts (chainsaw 🪚, frog 🐸) shown as pulsating markers at the originating node
  - A simulation timeline and event log overlay
- **AND** the output video is saved to `sim_output/musa_simulation.mp4` with a minimum resolution of 1920×1080 at 30 FPS

#### Scenario: Real-Time Satellite-Like Overlay
- **WHEN** the visualizer renders the map
- **THEN** it includes a stylized forest background with contour lines representing the MUSA reserve terrain
- **AND** LoRa radio coverage circles are drawn around each node to visualize communication range
- **AND** a status dashboard panel shows live battery, RSSI, and detection counts per node
