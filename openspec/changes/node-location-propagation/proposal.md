## Why

Currently, the sensor network lacks precise geolocalization for individual nodes, which is critical for mapping acoustic detection events like chainsaw activity and biodiversity sounds. By using a single gateway with precise geolocation and a low-power propagation algorithm, the entire network can be precisely mapped without the cost and power consumption of individual GPS modules on every leaf node.

## What Changes

- **Algorithm Specification**: Research and specify a state-of-the-art low-power localization propagation algorithm (e.g., modified DV-Hop or RSSI-based trilateration).
- **Node-to-Node Propagation**: Implementation of the selected algorithm within the ESP32-S3 firmware to calculate coordinates for all mesh nodes.
- **Acoustic Event Localization**: Development of an algorithm to localize the source of detected sounds based on multi-node detection timestamps or signal strength.
- **Metadata Enhancement**: Integration of GPS coordinates into the LoRaWAN/LoRa mesh telemetry packets.
- **Upstream Updates**: Modifications to the gateway and web-manager-app to process and display the propagated locations and event sources.
- **Documentation**: Update `system-archtecture-documentation` with the new localization logic and protocols.

## Capabilities

### New Capabilities
- `node-localization-propagation`: Handles the recursive calculation of node coordinates starting from the gateway's ground truth.
- `acoustic-sound-localization`: Calculates the source coordinates of detected acoustic events (chainsaws, wildlife) using data from multiple observing nodes.

### Modified Capabilities
- `lorawan-mesh-routing`: Update message schemas to include node coordinates and event source coordinates.
- `network-topology-management`: Enhance topology management to include spatial coordinates for each node ID.

## Impact

- **Firmware (ESP32-S3)**: New modules for localization math and coordination.
- **Protocol**: LoRa/LoRaWAN packet structure changes.
- **Backend/Web**: `web-manager-app` mapping component will now receive dynamic node and event positions.
- **Documentation**: Significant updates to the system architecture specs.
