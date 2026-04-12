#!/bin/bash

# Simulated Data Flow for Node Location Propagation
# 1. Gateway (Node 5) publishes its anchor coordinates
# 2. Node 1 (1-hop) receives and estimates distance
# 3. Node 2 (2-hop) receives from Node 1 and GW
# 4. Multi-node detection triggers WCL on Supabase

GATEWAY_URL="http://localhost:54321/functions/v1/ingest-mesh-data"

echo "--- STAGE 1: Gateway Anchor Broadcast ---"
curl -X POST $GATEWAY_URL \
  -H "Content-Type: application/json" \
  -d '{
    "node_id": "5",
    "status": "online",
    "battery_mv": 4200,
    "rssi_dbm": 0,
    "latitude": -23.5525,
    "longitude": -46.6355
  }'

echo -e "\n--- STAGE 2: Node 1 Detection at (-23.5520, -46.6350) ---"
curl -X POST $GATEWAY_URL \
  -H "Content-Type: application/json" \
  -d '{
    "node_id": "1",
    "status": "detecting",
    "battery_mv": 3900,
    "rssi_dbm": -85,
    "confidence": 0.92,
    "sound_class": "chainsaw",
    "latitude": -23.5520,
    "longitude": -46.6350
  }'

echo -e "\n--- STAGE 3: Node 2 Detection at (-23.5530, -46.6360) ---"
curl -X POST $GATEWAY_URL \
  -H "Content-Type: application/json" \
  -d '{
    "node_id": "2",
    "status": "detecting",
    "battery_mv": 3850,
    "rssi_dbm": -92,
    "confidence": 0.88,
    "sound_class": "chainsaw",
    "latitude": -23.5530,
    "longitude": -46.6360
  }'

echo -e "\n--- Verification: Localized event should appear between Node 1 and Node 2 ---"
