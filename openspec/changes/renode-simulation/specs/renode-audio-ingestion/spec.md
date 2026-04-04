## ADDED Requirements

### Requirement: Ingest Simulated Audio
The simulation framework SHALL provide a mechanism to feed simulated acoustic data (e.g., .wav file buffers) into the virtual node's audio subsystem.

#### Scenario: Audio detection trigger
- **WHEN** a chainsaw audio file is ingested by the simulation framework into Node 1
- **THEN** the firmware running in Node 1 processes the audio buffer and registers a detection event
