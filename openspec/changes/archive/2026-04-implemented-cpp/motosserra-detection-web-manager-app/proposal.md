## Why

Managing a decentralized network of Guardian nodes in a forest environment is currently a manual process involving static configuration files and limited feedback via Telegram. A centralized Web Manager App is needed to provide real-time visibility into the mesh network's health, visualize the geographic distribution of detections, and allow for remote configuration of sensor nodes to optimize battery life and detection accuracy.

## What Changes

- **Centralized Dashboard**: A React-based web interface using Material UI for a professional, responsive monitoring experience.
- **Serverless Backend**: Utilization of Supabase and Supabase Edge Functions for real-time data ingestion, authentication, and logic.
- **Vercel Deployment**: Optimized configuration for seamless deployment to Vercel's edge network.
- **Data Persistence Layer**: Migration of detection events to Supabase PostgreSQL for historical analysis and reporting.

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
