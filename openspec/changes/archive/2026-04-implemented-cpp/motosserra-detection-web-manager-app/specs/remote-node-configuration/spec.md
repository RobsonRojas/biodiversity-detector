## ADDED Requirements

### Requirement: Parameter Broadcasting
The system SHALL provide an interface to modify node parameters (e.g., detection threshold, active/sleep interval) and broadcast these changes to the network.

#### Scenario: Updating sensitivity
- **WHEN** the user changes the "High Sensitivity" toggle for a specific sector
- **THEN** the system SHALL queue a configuration packet and transmit it via the Gateway to all target nodes.

### Requirement: Configuration Confirmation
The system SHALL track and display the status of configuration updates (Pending, Applied, Failed).

#### Scenario: Successful update
- **WHEN** a node receives a new configuration and sends back an ACK
- **THEN** the Web Manager SHALL update that node's config status to "Applied" with a timestamp.
