# Capability: mesh-network-simulator

## Requirements

### Requirement: virtual-lora-air
The simulator MUST provide a UDP-based communication layer that mimics LoRa packet exchange.

#### Scenario: packet-broadcast
- **WHEN** a node sends a mesh packet
- **THEN** the packet is encapsulated in a UDP datagram and sent to the hostnames of its peer neighbors.

### Requirement: multi-hop-routing-v5
The system MUST support a 5-node static routing topology with a maximum depth of 4 hops.

#### Scenario: end-to-end-forwarding
- **WHEN** Node 1 (Leaf) initiates a detection
- **THEN** the packet hops through Node 2, Node 3, and Node 4 to reach Node 5 (Gateway).

### Requirement: trigger-simulation
The simulator MUST allow triggering a detection event via a filesystem signal or a mock audio file.

#### Scenario: chainsaw-detection-trigger
- **WHEN** a "trigger" signal is detected (e.g., presence of a file)
- **THEN** the `DetectionEngine` simulates a positive detection with matching confidence.
