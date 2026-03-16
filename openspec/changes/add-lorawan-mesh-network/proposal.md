## Why

A single detector's range is limited by forest density and terrain. To protect large areas of the Amazon Rainforest, we need a network of detectors that can relay information between each other. This change implements a LoRaWAN Mesh network that allows "leaf" detectors to relay detection events through "router" nodes until they reach a gateway with internet access for Telegram notification. This increases system resilience and dramatically expands the monitored area.

## What Changes

1.  **Mesh Protocol Layer**: Implementation of a decentralized routing protocol (inspired by Reticulum) over the LoRaWAN physical layer, optimized for pre-configured static routes.
2.  **Node Roles**: Introduction of `Leaf` (detection only), `Router` (detection + relay), and `Gateway` (relay + internet) roles.
3.  **Static Routing Logic**: Logic to optimize battery life by minimizing hops based on a pre-defined network topology.
4.  **Packet Encapsulation**: Modern C++ 26 implementation of packet serialization/deserialization for Mesh headers.

## Capabilities

### New Capabilities
- `lorawan-mesh-routing`: Multi-hop message relaying between detectors using static routing tables.
- `static-topology-management`: Configuration and validation of pre-defined network routes.

### Modified Capabilities
- `telegram-notifications`: Updated to accept alerts from networked devices, not just the local engine.

## Impact

- **Telemetry System**: Shifts from point-to-point LoRaWAN to a mesh topology.
- **Power Constraints**: Relaying packets consumes additional energy; `Router` nodes will require optimized duty cycles or larger solar panels.
- **Software Stack**: Heavy use of C++ 26 `std::expected` and `std::span` for packet processing.
