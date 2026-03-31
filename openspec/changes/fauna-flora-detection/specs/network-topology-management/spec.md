## MODIFIED Requirements

### Requirement: static-role-enforcement
The system MUST enforce node roles (Leaf, Router, Gateway) as defined in the static configuration, and follow the specified operational mode (detect-only vs forward-audio).

#### Scenario: leaf-no-relay
- **WHEN** a node configured as 'leaf' receives a packet for relay
- **THEN** it MUST discard the packet and prioritize detection logic

#### Scenario: respect-operation-mode
- **WHEN** a node is in 'detect-only' mode
- **THEN** it MUST ONLY send the detection alert header and NOT attempt to send audio fragments
