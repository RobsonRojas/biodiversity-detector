## Context

The Guardian system needs a sophisticated management interface to transition from a collection of isolated nodes to a manageable distributed sensor network. The Web Manager App will serve as the command-and-control center for forest monitoring.

## Goals / Non-Goals

**Goals:**
- Provide a high-fidelity geographic visualization of the mesh network.
- Enable remote heartbeat monitoring and battery status tracking.
- Allow researchers/operators to adjust detection sensitivity and sleep cycles remotely.
- Provide a queryable history of chainsaw detection events with peak audio analysis.

**Non-Goals:**
- Real-time audio streaming from all nodes (bandwidth constrained).
- Autonomous drone deployment/routing logic.
- Direct node-to-node management from the web (must go through the Gateway).

## Decisions

- **Framework**: **Next.js (App Router)**. Rationale: Provides a professional-grade SEO-ready foundation with excellent developer experience for building complex dashboards.
- **Styling**: **TailwindCSS with Shadcn/UI**. Rationale: Enables rapid development of a premium-feeling, responsive, and accessible interface.
- **State Management**: **React Query (TanStack Query)**. Rationale: Ideal for handling server-state, caching, and background synchronization with the Guardian API.
- **Backend API**: **FastAPI (Python)**. Rationale: Highly performant, easy to containerize, and has excellent libraries for geographic data (GeoAlchemy2) and real-time communication (WebSockets).
- **Network-Gateway Bridge**: **MQTT**. Rationale: The Gateway will act as an MQTT client, publishing mesh status updates and subscribing to configuration change topics. This decouples the web app from the physical LoRa network constraints.

## Risks / Trade-offs

- [Risk] LoRa Latency → [Mitigation] The UI will use "Pending Change" states and optimistic updates, acknowledging that a configuration change might take several mesh hops and sleep cycles to take effect.
- [Risk] Massive Data Volume → [Mitigation] Detection history will be aggregated and indexed by geographic "sectors" to maintain dashboard performance.
