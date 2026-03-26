## Why

Managing a decentralized network of Guardian nodes in a forest environment is currently a manual process involving static configuration files and limited feedback via Telegram. A centralized Web Manager App is needed to provide real-time visibility into the mesh network's health, visualize the geographic distribution of detections, and allow for remote configuration of sensor nodes to optimize battery life and detection accuracy.

## What Changes

- **Centralized Dashboard**: A web-based interface for real-time monitoring of all Guardian nodes (Leaf, Router, Gateway).
- **Network Topology Visualization**: An interactive map and graph view showing node connections, signal strength (RSSI), and battery levels.
- **Remote Configuration API**: A backend service to push configuration updates (e.g., detection thresholds, sleep intervals) from the web app through the Gateway into the mesh network.
- **Data Persistence Layer**: Migration of detection events from ephemeral logs to a persistent database for historical analysis and reporting.

## Capabilities

### New Capabilities
- `network-topology-visualizer`: Interactive visualization of the mesh network structure and geographic node placement.
- `remote-node-configuration`: System to broadcast configuration changes to specific nodes or groups via the LoRa mesh.
- `detection-history-analytics`: Real-time and historical data processing for trend analysis and heatmaps.

### Modified Capabilities
- `network-topology-management`: Extended to report RSSI and battery status to the Gateway for upstream web consumption.

## Impact

- **Infrastructure**: Requires a web server and database (e.g., Supabase or a custom FastAPI/PostgreSQL stack).
- **Gateway Node**: The Gateway's firmware must be updated to forward mesh status packets to the new Backend API.
- **Connectivity**: Requires internet access at the Gateway or a local server within the same network.
