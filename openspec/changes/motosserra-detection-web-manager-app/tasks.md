## 1. Project Setup

- [x] 1.1 Re-initialize frontend using Vite and install `react-router-dom`, `@mui/material`, `@emotion/react`, `@emotion/styled`.
- [x] 1.2 Initialize Supabase project using the Supabase CLI (`supabase init`).
- [x] 1.3 Configure `vercel.json` for deployment settings.
- [ ] 1.4 Set up local Supabase development environment in Docker.

## 2. Supabase & Edge Functions

- [x] 2.1 Define SQL schema for `nodes` and `detections` tables in Supabase.
- [x] 2.2 Create `ingest-mesh-data` Supabase Edge Function to handle Gateway webhooks.
- [x] 2.3 Create `broadcast-config` Edge Function to queue mesh configuration changes.
- [x] 2.4 Enable Row Level Security (RLS) and real-time subscriptions for the dashboard.

## 3. Frontend Implementation (React & MUI)

- [x] 3.1 Implement MUI Theme Customization for a high-premium forest monitoring aesthetic.
- [x] 3.2 Build interactive Map Dashboard using `react-leaflet` within an MUI container.
- [x] 3.3 Create Real-time Node Status components using MUI `Card` and `DataGrid`.
- [x] 3.4 Build Configuration forms using MUI `TextField` and `Slider` components.
- [x] 3.5 Implement Analytics view with detection heatmaps and trend charts.

## 4. Gateway Integration

- [x] 4.1 Update Raspberry Pi Gateway software to POST mesh packets to the Supabase Edge Function URL.
- [x] 4.2 Validate end-to-end data flow: Mesh Node -> Gateway -> Supabase -> Next.js (Real-time).

## 5. Verification

- [x] 5.1 Run unit tests for Backend API and Data Models.
- [x] 5.2 Perform end-to-end integration test with a simulated mesh node.
- [x] 5.3 Verify geographic plotting and RSSI color-coding on the Map Dashboard.
