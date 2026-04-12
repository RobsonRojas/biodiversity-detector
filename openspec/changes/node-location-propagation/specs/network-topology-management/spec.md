## ADDED Requirements

### Requirement: spatial-topology-tracking
The system MUST maintain a record of the $(x, y)$ or $(\text{lat}, \text{lon})$ coordinates for each node in the mesh.

#### Scenario: coordinate-update
- **WHEN** a node reports a new calculated location to the gateway
- **THEN** the gateway MUST update its local topology record with the new coordinates

## MODIFIED Requirements

### Requirement: static-role-enforcement
The system MUST enforce node roles (Leaf, Router, Gateway) and node locations for the purpose of geographical visualization.

#### Scenario: leaf-no-relay
- **WHEN** a node configured as 'leaf' receives a packet for relay
- **THEN** it MUST discard the packet and prioritize detection logic, including localized acoustic analysis
