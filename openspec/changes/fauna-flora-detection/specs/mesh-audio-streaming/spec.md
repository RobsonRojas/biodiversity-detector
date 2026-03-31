## ADDED Requirements

### Requirement: audio-packet-fragmentation
The system SHALL fragment captured audio clips (up to 5 seconds) into smaller packets suitable for LoRa mesh transmission (max 200 bytes per packet).

#### Scenario: send-audio-clip
- **WHEN** a detection occurs in 'forward-audio' mode
- **THEN** the node MUST send a sequence of numbered mesh packets containing the raw or compressed audio data

### Requirement: best-effort-reassembly
The gateway SHALL reassemble incoming audio fragments based on their sequence numbers and node ID.

#### Scenario: missing-fragments
- **WHEN** some audio fragments are lost during mesh relay
- **THEN** the gateway SHOULD still attempt to reconstruct the audible portions or signal a partial delivery
