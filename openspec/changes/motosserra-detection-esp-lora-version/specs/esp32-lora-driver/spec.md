## ADDED Requirements

### Requirement: Raw LoRa Transmission (AU915)
The system SHALL transmit and receive raw MeshPacket structures using the AU915 frequency band.

#### Scenario: Send alert
- **WHEN** a chainsaw is detected
- **THEN** the LoRa driver transmits the serialized MeshPacket over the air

### Requirement: Asynchronous Reception
The system SHALL listen for incoming mesh packets without blocking the main event loop.

#### Scenario: Receive relayed packet
- **WHEN** a packet arrives on the configured frequency
- **THEN** an interrupt triggers the reception callback to process the MeshPacket
