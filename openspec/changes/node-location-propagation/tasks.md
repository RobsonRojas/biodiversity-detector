## 1. Node Localization Implementation

- [ ] 1.1 Add `LocalizationModule` to the ESP32-S3 firmware
- [ ] 1.2 Implement Log-Distance Path Loss Model for distance estimation from RSSI
- [ ] 1.3 Implement Iterative Multi-lateration (IML) logic using Least Squares
- [ ] 1.4 Update `EspLoRaDriver` to expose RSSI values for every received packet
- [ ] 1.5 Integrate localization beacons into the heartbeat broadcast cycle

## 2. Acoustic Localization Implementation

- [ ] 2.1 Implement multi-node detection correlation logic on the Gateway
- [ ] 2.2 Implement Weighted Centroid Localization (WCL) for acoustic events
- [ ] 2.3 Store source coordinates in the gateway's detection log

## 3. Telemetry and Protocols

- [ ] 3.1 Update LoRa packet structure to include location metadata (64-bit lat/lon)
- [ ] 3.2 Update `telemetry_manager` to pack/unpack location data
- [ ] 3.3 Add location fields to the Gateway API (REST/Menejo-API)
- [ ] 3.4 Update Web Manager App mapping component to plot dynamic node positions

## 4. Documentation and Verification

- [ ] 4.1 Update `system-archtecture-documentation` with the new localization logic
- [ ] 4.2 Create unit tests for IML and WCL math
- [ ] 4.3 Verify end-to-end propagation in a 3-node laboratory test mesh
