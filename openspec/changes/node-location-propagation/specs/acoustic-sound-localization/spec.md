# acoustic-sound-localization Specification

## Purpose
Enable the localization of acoustic event sources (chainsaws, wildlife) by correlating detections across multiple nodes in the mesh.

## ADDED Requirements

### Requirement: multi-node-correlation
The gateway MUST correlate detection events with overlapping timestamps (within 10s window) from different nodes.

#### Scenario: chainsaw-event-correlation
- **WHEN** the gateway receives a 'chainsaw-detected' packet from 2 or more nodes
- **THEN** it MUST group them as a single acoustic event for localization

### Requirement: weighted-centroid-localization
The system MUST calculate the source coordinates using a weighted centroid based on detection confidence and known node coordinates.

#### Scenario: localization-calculation
- **WHEN** a correlated event has been identified
- **THEN** its (x,y) location MUST be calculated as the weighted average of the observer nodes' positions

### Requirement: upstream-telemetry
The gateway MUST append the source coordinates to its reports to the web-manager-app and Telegram.

#### Scenario: send-to-telegram-with-map-link
- **WHEN** an acoustic event is localized
- **THEN** the Telegram alert MUST include a link to a Google Map at the calculated coordinates
