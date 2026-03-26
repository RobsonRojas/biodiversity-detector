## Context

Current detectors are isolated. This design introduces a Mesh network layer that allows detectors to communicate using LoRaWAN AU915, enabling alerts to reach a central gateway even if the direct path to the gateway is blocked by terrain or distance.

## Goals / Non-Goals

**Goals:**
- Implement a static routing protocol optimized for low-power LoRa.
- Use C++ 26 features (`std::span`, `std::expected`) for packet handling.
- Support pre-configured static routing tables based on known node locations.
- Ensure message integrity with packet signatures.

**Non-Goals:**
- Implementation of dynamic neighbor discovery or beaconing.
- Auto-optimization of routes during runtime (topology is fixed at deployment).
- High-bandwidth data transfer (e.g., sending raw audio clips).

## Decisions

- **Static Routing Table**: Simpler and more power-efficient. Routes are burned into the firmware or provided via protected configuration during deployment.
- **Pre-calculated Metrics**: Link quality (SNR) is measured during deployment and used to pre-calculate the best static routes.
- **std::flat_map**: Used for the routing table to minimize memory allocations and improve cache locality on the RPi Zero.
- **Zero-Copy Parsers**: Using `std::span` to interpret LoRa buffers directly, avoiding heap allocations during relaying.

## Risks / Trade-offs

- **Topology Drift**: If a physical obstacle (tree growth, fallen branch) significantly changes SNR, a static route might fail. The system relies on manual re-configuration in such cases.
- **Deployment Complexity**: Requires precise planning of node placement and routing before deployment.
- **Latency**: Each hop adds latency (~1s per hop). Total alert delivery time could reach 10s in deep mesh, which is acceptable for forestry response.
