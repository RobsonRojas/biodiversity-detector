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

- **Framework**: **Vite + React**. Rationale: Provides a faster development cycle and more flexibility for standard React SPAs compared to Next.js when SSR is not required.
- **Routing**: **React Router**. Rationale: Industry standard for client-side routing in React applications, allowing for complex nested layouts and navigation.
- **Styling**: **Material UI (MUI)**. Rationale: Requirement for a robust, component-driven design system with built-in accessibility and a professional look.
- **Data Layer**: **Supabase**. Rationale: Provides managed PostgreSQL, Authentication, and Realtime subscriptions out-of-the-box, significantly reducing backend overhead.
- **Business Logic**: **Supabase Edge Functions (Deno/TypeScript)**. Rationale: Scalable, serverless logic for processing mesh gateway events and configuration broadcasts.
- **Deployment**: **Vercel**. Rationale: Native support for Next.js, providing instant deployments and edge performance.
- **Network-Gateway Bridge**: **Edge Function Webhooks**. Rationale: The Gateway will post mesh updates directly to a Supabase Edge Function via HTTPS, which then updates the DB and triggers real-time UI refreshes.

## Risks / Trade-offs

- [Risk] LoRa Latency → [Mitigation] The UI will use "Pending Change" states and optimistic updates, acknowledging that a configuration change might take several mesh hops and sleep cycles to take effect.
- [Risk] Massive Data Volume → [Mitigation] Detection history will be aggregated and indexed by geographic "sectors" to maintain dashboard performance.
