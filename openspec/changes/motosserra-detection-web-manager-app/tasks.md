## 1. Project Setup

- [ ] 1.1 Initialize Next.js frontend with TailwindCSS and Lucide icons in `src/web-manager/frontend`.
- [ ] 1.2 Initialize FastAPI backend in `src/web-manager/backend`.
- [ ] 1.3 Configure PostgreSQL/PostGIS database schema for nodes and detection events.
- [ ] 1.4 Set up Docker Compose for local development (Web, API, DB).

## 2. Backend & Communication

- [ ] 2.1 Implement Geographic models for nodes and detection heatmaps.
- [ ] 2.2 Create API endpoints for fetching node status, connectivity, and historical detections.
- [ ] 2.3 Implement MQTT bridge to receive mesh status updates from the Gateway.
- [ ] 2.4 Implement Remote Configuration service to publish parameter updates over MQTT.

## 3. Frontend Implementation

- [ ] 3.1 Build interactive Map Dashboard using `react-leaflet` or `vis.js` for topology.
- [ ] 3.2 Implement Real-time Node Status sidebar with battery level and RSSI indicators.
- [ ] 3.3 Create Remote Configuration panel for updating node sensitive parameters.
- [ ] 3.4 Implement Analytics view with detection heatmaps and trend charts.

## 4. Gateway Integration

- [ ] 4.1 Update Raspberry Pi Gateway software to publish mesh packets to the MQTT broker.
- [ ] 4.2 Validate end-to-end data flow from Mesh Node -> Gateway -> MQTT -> FastAPI -> Web.

## 5. Verification

- [ ] 5.1 Run unit tests for Backend API and Data Models.
- [ ] 5.2 Perform end-to-end integration test with a simulated mesh node.
- [ ] 5.3 Verify geographic plotting and RSSI color-coding on the Map Dashboard.
