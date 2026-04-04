## 1. Setup Simulation Environment

- [x] 1.1 Create the main Renode script `mesh-sim.resc` integrating a single Raspberry Pi 3B node model.
- [x] 1.2 Validate that the target-compiled `guardian_node` firmware binary successfully boots inside the simulated node.

## 2. LoRa Mesh Simulation

- [x] 2.1 Add a `WirelessMedium` configuration to the `.resc` script to simulate the RF environment.
- [x] 2.2 Expand the `.resc` file to provision 5 individual nodes (1 Leaf, 3 Routers, 1 Gateway).
- [x] 2.3 Connect the simulated SPI LoRa peripherals of all 5 nodes to the shared `WirelessMedium`.
- [x] 2.4 Verify proper network topology and basic routing of mesh heartbeat packets within the simulation.

## 3. Audio Ingestion Simulation

- [x] 3.1 Determine the appropriate Renode mechanism for audio ingestion (e.g. native I2S model, file-backed memory buffer, or a custom Python peripheral proxy).
- [x] 3.2 Implement the chosen ingestion method to feed external `.wav` files into the audio pipeline of simulated Node 1.
- [x] 3.3 Validate that the firmware's AI engine detects the simulated acoustic patterns correctly.

## 4. End-to-End Verification

- [x] 4.1 Execute the full multi-node simulation starting from a clean state.
- [x] 4.2 Trigger the audio ingestion mechanism on Node 1 using the provided chainsaw simulation audio.
- [x] 4.3 Validate that the resulting alert correctly hops through the virtual radio layer from Node 1 up to Node 5.
