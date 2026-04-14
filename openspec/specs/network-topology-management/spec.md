# network-topology-management Specification

## Purpose
Define how the Guardian mesh network manages node roles, hierarchy, and power states to ensure long-term autonomy in the forest environment.

## Requirements

### Requirement: dynamic-role-assignment
The system MUST support the assignment of roles (Leaf, Router, Gateway) to optimize the mesh topology and battery usage.

#### Scenario: leaf-power-efficiency
- **GIVEN** a node is configured as a `Leaf`
- **WHEN** it is in monitoring mode
- **THEN** it MUST prioritize acoustic detection and AI inference
- **THEN** it MUST NOT relay packets for other nodes to preserve battery.

#### Scenario: router-autonomy
- **GIVEN** a node is configured as a `Router`
- **WHEN** it is in active mode
- **THEN** it MUST stay in `light_sleep` between LoRa windows to listen for relay requests
- **THEN** it MUST maintain a `routing_table` of its direct neighbors.

### Requirement: topology-visualization
The system MUST provide enough metadata in telemetry to allow the `web-manager-app` to reconstruct the network graph.

#### Scenario: mesh-graph-data
- **WHEN** a node relays a packet
- **THEN** it MUST append its `node_id` to the `hop_history` (metadata) if the payload size allows.
