## 1. Simulated Physical Layer

- [ ] 1.1 Implement `SimulatedLoRaDriver.cpp` using UDP sockets for inter-container communication.
- [ ] 1.2 Add environment variable support for `SIM_LORA_PORT` and `SIM_LORA_NEIGHBORS`.
- [ ] 1.3 Ensure `LoRaDriver` can switch between `SX1262` (physical) and `UDP` (simulated) at compile-time or start-time.

## 2. Multi-Hop Topology & Routing

- [ ] 2.1 Define static route JSON files for each of the 5 nodes in the linear topology.
- [ ] 2.2 Update `AlertManager` to log hop metadata for verification purposes.
- [ ] 2.3 Implement a heartbeat mechanism to verify mesh connectivity across 4 hops.

## 3. Docker Orchestration

- [ ] 3.1 Update `Dockerfile` to produce a simulation-ready binary.
- [ ] 3.2 Implement `docker-compose.sim.yml` with 5 node services and a shared backend network.
- [ ] 3.3 Create a `mocks/` directory with a 1-second chainsaw audio sample for trigger injection.

## 4. End-to-End Verification

- [ ] 4.1 Launch the 5-node mesh and verify peer-to-peer UDP connectivity.
- [ ] 4.2 Trigger a detection on Node 1 (most remote) and trace the packet through Nodes 2, 3, and 4.
- [ ] 4.3 Verify that the detection reaches Supabase and triggers a Telegram alert from Node 5 (Gateway).
