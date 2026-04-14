## Context

Capturing the behavior of a multi-hop mesh network in a local environment requires a virtualized physical layer. Since we are using Docker, we will replace the SPI-based LoRa driver with a UDP-based simulation layer that allows containers to "broadcast" packets to their neighbors over a dedicated Docker bridge network.

## Goals / Non-Goals

**Goals:**
- Simulate a 5-node linear/tree topology with up to 4 hops.
- Re-use the existing C++ `guardian_node` logic with minimal modifications (conditional compilation or abstraction layer).
- Verify end-to-end delivery from Node 1 to Supabase/Telegram via Node 5.

**Non-Goals:**
- Simulating RF interference, path loss, or fading (using "perfect air").
- Simulating LoRaWAN Join Server or Duty Cycle (focusing on the raw mesh protocol).

## Decisions

- **UDP Simulation Layer**: Introduce a `SimulatedLoRaDriver` that implements the `ILoRaDriver` interface. It will send/receive UDP packets on the Docker network.
- **Topology Configuration**: Use a JSON-based static routing table for each node, passed via Docker environment variables.
- **Container Orchestration**: Use `docker-compose.yml` with 5 services, each mapped to a specific node ID and role.
- **Mock Audio Trigger**: Use a pre-recorded `.wav` file of a chainsaw mounted as `/app/audio/trigger.wav` in the containers to simulate real-time detection events.

## Risks / Trade-offs

- **Synchronization**: Docker networking might introduce jitter not present in real LoRa (125kHz bandwidth is much slower than Ethernet). → Mitigation: Add artificial delays in the `SimulatedLoRaDriver`.
- **Port Management**: Each container needs a unique UDP port or we must use container hostnames. → Decision: Use container hostnames (`node1`, `node2`, etc.) and a fixed port (e.g., 5000).
