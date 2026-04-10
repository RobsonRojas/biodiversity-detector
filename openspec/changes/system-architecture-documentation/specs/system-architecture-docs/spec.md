## ADDED Requirements

### Requirement: c4-model-documentation
The architecture MUST be documented using the C4 model (Context, Container, and Component).

#### Scenario: system-context-clarity
- **WHEN** a user views the architecture documentation
- **THEN** they must see a System Context diagram showing how the detector interacts with the physical environment, the mesh network, and backend systems.

#### Scenario: component-level-detail
- **WHEN** a developer investigates the firmware internals
- **THEN** they must see Component diagrams for Audio, AI, and Telemetry modules.

### Requirement: uml-visualization
The documentation MUST include standard UML diagrams for deep-dive technical understanding.

#### Scenario: class-hierarchy
- **WHEN** reviewing the Alert Management system
- **THEN** a Class diagram must show the relationships between AlertManager, Clients (Telegram, Supabase), and the OfflineQueue.

#### Scenario: data-flow-sequence
- **WHEN** analyzing the detection-to-alert latency
- **THEN** a Sequence diagram must show the call graph from I2S data capture through spectral analysis, AI inference, and LoRa transmission.
