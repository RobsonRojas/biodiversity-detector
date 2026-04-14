## ADDED Requirements

### Requirement: Detection Heatmap
The system SHALL generate a geographic heatmap based on historical detection events and their reported confidence levels.

#### Scenario: Identifying high-risk areas
- **WHEN** the user selects the "Heatmap" view for the last 30 days
- **THEN** the map SHALL display intensity gradients color-coded by the density of chainsaw detections.

### Requirement: Event Log Export
The system SHALL allow users to filter and export detection logs in CSV or JSON format for external reporting.

#### Scenario: Exporting monthly report
- **WHEN** the user applies a date filter and clicks "Export CSV"
- **THEN** the system SHALL download a file containing node IDs, timestamps, and confidence scores.
