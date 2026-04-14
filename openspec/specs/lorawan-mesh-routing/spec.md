# lorawan-mesh-routing Specification

## Purpose
Define the mesh communication protocol used by Guardian nodes to propagate alerts and telemetry from deep forest locations to the central gateway via multi-hop relaying.

## Requirements

### Requirement: multi-hop-alert-propagation
The system MUST forward alert messages (Chainsaw/Fauna) from Leaf nodes to the Gateway using Routers as intermediate hops.

#### Scenario: relay-with-low-latency
- **WHEN** a router node receives a packet with `destination_id == GATEWAY`
- **THEN** it forwards the packet to the next optimal hop in < 500ms
- **THEN** it updates its internal `routing_table` with the RSSI of the sender.

### Requirement: location-propagation
The system MUST calculate and propagate geographic coordinates (Lat/Lon) to nodes that lack a GPS module.

#### Scenario: gateway-seed
- **GIVEN** the Gateway has authoritative GPS coordinates
- **WHEN** a node joins the mesh
- **THEN** the Gateway or a calibrated Neighbor propagates its coordinates to the new node
- **THEN** the node uses the RSSI/ToF to estimate its own position.

### Requirement: telemetry-payload
Telemetry MUST be transmitted in a compact JSON format to minimize LoRa airtime.

#### Scenario: payload-structure
- **WHEN** sending telemetry
- **THEN** include `node_id`, `battery`, `lat`, `lon`, `detections_count`, and `uptimes`.
