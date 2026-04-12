## Context

The system currently lacks a way to determine the geographic coordinates of leaf nodes and the source of detected acoustic events. While the gateway has a high-precision GPS lock, leaf nodes are deployed without GPS to minimize power and cost. This design leverages the LoRa mesh network to propagate location from the gateway to all nodes and uses multi-node telemetry to localize sound sources.

## Goals / Non-Goals

**Goals:**
- Propagate precise (lat, lon) from the gateway anchor to all leaf nodes using RSSI-based iterative multi-lateration.
- Localize chainsaw and biodiversity sounds within a 50m error radius using weighted centroid correlation.
- Integrate location data into the LoRaWAN payload and upstream alerts (Telegram/Web).
- Maintain ultra-low power consumption for leaf nodes.

**Non-Goals:**
- High-precision (cm-level) localization using Time-of-Flight (ToF).
- Real-time tracking of moving objects.
- Individual GPS modules on leaf nodes.

## Decisions

### 1. Algorithm: Iterative Multi-lateration (IML) for Node Positions
- **Rationale**: IML allows the network to "self-map" starting from the gateway. 1-hop nodes estimate distance from the gateway using RSSI. Once 3 nodes have coordinates, they become virtual anchors for nodes further out.
- **Alternative considered**: DV-Hop (Distance Vector-Hop). *Rejected* because it is less accurate for small-scale meshes with non-uniform node distribution.

### 2. Distance Estimation: RSSI Path Loss Model
- **Rationale**: The `EspLoRaDriver` already provides `get_last_rssi()`. We use the Log-Distance Path Loss Model: $PL = PL_0 + 10n \log_{10}(d/d_0) + X_g$.
- **Alternative considered**: Time Difference of Arrival (TDOA). *Rejected* because LoRa chips (SX1262) do not support sub-microsecond synchronization without external PPS.

### 3. Acoustic Localization: Weighted Centroid Localization (WCL)
- **Rationale**: When multiple nodes detect a sound, the gateway calculates the source $(x, y)$ as $\sum (w_i \cdot P_i) / \sum w_i$, where $P_i$ is node $i$'s position and $w_i$ is detection confidence.
- **Alternative considered**: Acoustic Trilateration. *Rejected* because it requires very precise detection timestamps across nodes, which are difficult to guarantee in a low-power mesh.

## Risks / Trade-offs

- **[Risk] RSSI Fluctuations** → **[Mitigation]** Use exponential moving average (EMA) for RSSI values over multiple heartbeat packets to filter out multipath interference.
- **[Risk] Forest Canopy Attenuation** → **[Mitigation]** Tune the path loss exponent $n$ based on empirical "jungle" environment measurements stored in the firmware config.
- **[Risk] Cascading Errors in IML** → **[Mitigation]** Limit the recursion depth or introduce a confidence score that decays with each hop from the physical anchor (gateway).
