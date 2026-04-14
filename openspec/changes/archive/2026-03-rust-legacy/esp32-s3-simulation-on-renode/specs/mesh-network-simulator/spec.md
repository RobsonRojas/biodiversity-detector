## MODIFIED Requirements

### Requirement: Multi-Node Backend Support
The mesh simulator SHALL support "Renode" as a backend type for leaf nodes in the `.env` or configuration file.

#### Scenario: Mixed-Mode Network
- **WHEN** Node 1 is a regular process and Node 2 is a Renode instance.
- **THEN** Node 1 should be able to receive detection alerts from Node 2 via the simulated mesh relay.
