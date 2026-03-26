# lorawan-mesh-routing Specification

## Purpose
TBD - created by archiving change add-lorawan-mesh-network. Update Purpose after archive.
## Requirements
### Requirement: static-message-forwarding
The system MUST forward messages between nodes based on pre-defined static routing paths.

#### Scenario: relay-to-gateway
- **WHEN** a router node receives a packet with a destination ID matching a gateway in its static table
- **THEN** it forwards the packet to the next hop hop with < 2s latency

