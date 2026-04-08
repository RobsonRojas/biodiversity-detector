## MODIFIED Requirements

### Requirement: static-message-forwarding
The system MUST forward messages between nodes based on pre-defined static routing paths, validating this logic through a heterogeneous 5-node simulation containing both native Linux nodes and at least one ESP32-S3 node emulated via Espressif QEMU.

#### Scenario: relay-to-gateway
- **WHEN** a router node receives a packet with a destination ID matching a gateway in its static table
- **THEN** it forwards the packet to the next hop with < 2s latency, specifically verified by logging successful delivery through an emulated ESP32-S3 hop
