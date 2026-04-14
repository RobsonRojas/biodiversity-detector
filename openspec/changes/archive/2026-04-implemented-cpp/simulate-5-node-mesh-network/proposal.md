## Why

The Guardian project requires rigorous validation of its multi-hop LoRa mesh network before field deployment. This 5-node simulation allows us to verify the static routing logic, packet integrity, and the end-to-end integration between remote detectors and the Web Manager App (Supabase) in a scalable, repeatable Docker environment.

## What Changes

- **5-Node Simulation Topology**: Orchestrate 5 Docker containers representing separate LoRa nodes (Leafs, Routers, and a Gateway).
- **Virtual LoRa Layer**: Implement a simulated "air" interface using a shared network or message broker to allow containers to exchange mesh packets.
- **Static Routing Configuration**: Define a multi-hop path (e.g., Node 1 -> Node 2 -> Node 3 -> Node 4 -> Node 5) to test router reliability.
- **End-to-End Monitoring**: Verify that a detection at the most remote node (Node 1) reaches the Gateway (Node 5) and triggers both Telegram notifications and Supabase database updates.

## Capabilities

### New Capabilities
- `mesh-network-simulator`: Orchestration, container-to-container LoRa simulation, and topology configuration for the multi-hop network.

### Modified Capabilities
- `network-topology-management`: Extended to support 5 nodes and complex multi-hop static routes.
- `network-topology-visualizer`: Support for visualizing paths with up to 4 hops.

## Impact

- **Build System**: Requires updates to `docker-compose.yml` to support 5 node instances.
- **Configuration**: New static route maps for the 5-node setup.
- **Verification**: New integration test scripts to trigger simulated detections and poll results from Supabase.
