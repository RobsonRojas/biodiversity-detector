# node-localization-propagation Specification

## Purpose
Define the process by which a single precise geolocation (gateway) is propagated across a low-power LoRa mesh to enable individual node coordinates for spatial mapping.

## ADDED Requirements

### Requirement: gateway-anchor-origin
The system MUST use the gateway node as the origin anchor for all localization calculations.

#### Scenario: gateway-init
- **WHEN** the gateway node boots with GPS lock
- **THEN** it MUST broadcast its precise coordinates to all 1-hop neighbor nodes

### Requirement: iterative-multi-lateration
Nodes MUST calculate their own coordinates using RSSI-based distance estimation from at least 3 anchor points (recursive).

#### Scenario: leaf-coordinate-propagation
- **WHEN** a leaf node receives coordinate beacons from 3 or more neighbors (anchors)
- **THEN** it MUST calculate its position using Least Squares and broadcast itself as a virtual anchor

### Requirement: low-power-background-update
Localization propagation MUST occur during standard mesh maintenance (heartbeats) to minimize power consumption.

#### Scenario: heartbeat-rssi-collection
- **WHEN** a node receives a standard mesh heartbeat
- **THEN** it MUST tag the packet with the current RSSI and pass it to the localization module
