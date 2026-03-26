# network-topology-management Specification

## Purpose
TBD - created by archiving change add-lorawan-mesh-network. Update Purpose after archive.
## Requirements
### Requirement: static-role-enforcement
The system MUST enforce node roles (Leaf, Router, Gateway) as defined in the static configuration.

#### Scenario: leaf-no-relay
- **WHEN** a node configured as 'leaf' receives a packet for relay
- **THEN** it MUST discard the packet and prioritize detection logic

