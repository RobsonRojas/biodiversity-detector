## ADDED Requirements

### Requirement: Geographic Node Plotting
The system SHALL display all known mesh nodes on an interactive map according to their last reported coordinates or static installation metadata.

#### Scenario: Visualizing network spread
- **WHEN** the user opens the Network Dashboard
- **THEN** an interactive map renders showing icons for Leaf, Router, and Gateway nodes at their approximate locations.

### Requirement: Real-time Link Status
The system SHALL visualize the connectivity between nodes, indicating link quality (RSSI) and hop distance.

#### Scenario: Identifying weak spots
- **WHEN** a link between a Leaf and a Router has an RSSI below -110dBm
- **THEN** the connecting line on the map SHALL change color to Red to alert the operator.
