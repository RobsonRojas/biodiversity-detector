## ADDED Requirements

### Requirement: location-metadata-propagation
The system MUST include coordinate metadata (uint32 latitude, uint32 longitude) in mesh-wide localization beacons.

#### Scenario: mesh-beacon-location
- **WHEN** a node broadcasts its localization status
- **THEN** it MUST include its currently calculated (x,y) or (lat,lon) in the packet header

## MODIFIED Requirements

### Requirement: static-message-forwarding
The system MUST forward messages between nodes based on pre-defined static routing paths, including any appended location telemetry.

#### Scenario: relay-to-gateway
- **WHEN** a router node receives a packet with a destination ID matching a gateway in its static table
- **THEN** it forwards the packet to the next hop hop with < 2s latency and preserves all source location data
