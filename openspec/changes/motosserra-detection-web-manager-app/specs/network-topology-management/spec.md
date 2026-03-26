## MODIFIED Requirements

### Requirement: Node Metadata Reporting
The existing `network-topology-management` SHALL be extended to include diagnostic metadata in the heartbeat packets.

#### Scenario: Reporting battery and RSSI
- **WHEN** a node sends a periodic heartbeat or detection packet
- **THEN** it SHALL include current battery voltage (mV) and the RSSI of the parent hop.
