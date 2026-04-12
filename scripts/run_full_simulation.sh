#!/bin/bash
# MUSA Forest Full Simulation Orchestrator
# Simulates: chainsaw detection, frog detection, telemetry via mesh, gateway reception
# Runs against docker-compose.sim.yml (5-node mesh network)

set -e

NODE1_CONTAINER="node1"
GATEWAY_CONTAINER="node5"
NODE1_ID="0x0001"
LOG_FILE="simulation_output.log"

echo "╔══════════════════════════════════════════════════╗"
echo "║  MUSA Forest - Full Simulation Orchestrator      ║"
echo "║  Location: Jardim Botânico de Manaus             ║"
echo "║  Mesh: 5 nodes (1 leaf, 3 routers, 1 gateway)   ║"
echo "╚══════════════════════════════════════════════════╝"
echo ""

# 0. Build and start the mesh network
echo "[STEP 0] Building and starting 5-node mesh network..."
docker compose -f docker-compose.sim.yml up -d --build 2>&1 | tail -5
echo ""

# 1. Wait for mesh convergence
echo "[STEP 1] Waiting 20s for mesh convergence..."
sleep 20
echo "Mesh network ready."
echo ""

# 2. Show initial heartbeat activity
echo "[STEP 2] Verifying nodes are alive..."
for n in node1 node2 node3 node4 node5; do
    echo -n "  $n: "
    docker logs "$n" 2>&1 | tail -1
done
echo ""

# 3. Trigger CHAINSAW detection on Node 1 (leaf)
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "[STEP 3] 🪚 Triggering CHAINSAW Detection on Node 1..."
docker exec "$NODE1_CONTAINER" touch "/tmp/${NODE1_ID}_chainsaw_trigger"
echo "  Waiting 15s for detection + mesh propagation..."
sleep 15

echo "  Node 1 detection logs:"
docker logs "$NODE1_CONTAINER" 2>&1 | grep -i -E "chainsaw|alert|trigger|DETECTED" | tail -5
echo ""
echo "  Gateway (Node 5) reception:"
docker logs "$GATEWAY_CONTAINER" 2>&1 | grep -i -E "chainsaw|alert|mesh_receive|DETECTED" | tail -5

# Clean up trigger
docker exec "$NODE1_CONTAINER" rm -f "/tmp/${NODE1_ID}_chainsaw_trigger" 2>/dev/null || true
echo ""

# 4. Trigger FROG (Boana geographica) detection on Node 1
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "[STEP 4] 🐸 Triggering Frog (Boana geographica) Detection on Node 1..."
docker exec "$NODE1_CONTAINER" touch "/tmp/${NODE1_ID}_frog_trigger"
echo "  Waiting 15s for detection + mesh propagation..."
sleep 15

echo "  Node 1 detection logs:"
docker logs "$NODE1_CONTAINER" 2>&1 | grep -i -E "frog|boana|amphibian|animal|alert|trigger|DETECTED" | tail -5
echo ""
echo "  Gateway (Node 5) reception:"
docker logs "$GATEWAY_CONTAINER" 2>&1 | grep -i -E "frog|boana|amphibian|animal|alert|mesh_receive|DETECTED" | tail -5

# Clean up trigger
docker exec "$NODE1_CONTAINER" rm -f "/tmp/${NODE1_ID}_frog_trigger" 2>/dev/null || true
echo ""

# 5. Check telemetry / heartbeat propagation
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "[STEP 5] 📡 Checking telemetry propagation across mesh..."
echo ""
for n in node1 node2 node3 node4 node5; do
    echo "  ── $n ──"
    docker logs "$n" 2>&1 | grep -i -E "heartbeat|telemetry|mesh|send|receive" | tail -3
    echo ""
done

# 6. Full log dump for analysis
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "[STEP 6] Saving full logs to $LOG_FILE..."
for n in node1 node2 node3 node4 node5; do
    echo "=== $n ===" >> "$LOG_FILE"
    docker logs "$n" 2>&1 >> "$LOG_FILE"
    echo "" >> "$LOG_FILE"
done

# Save individual logs
mkdir -p sim_logs
for n in node1 node2 node3 node4 node5; do
    docker logs "$n" 2>&1 > "sim_logs/${n}.log"
done
echo "  Individual logs saved to sim_logs/"
echo ""

echo "╔══════════════════════════════════════════════════╗"
echo "║  ✓ Simulation Complete                           ║"
echo "║  Logs: $LOG_FILE, sim_logs/                      ║"
echo "╚══════════════════════════════════════════════════╝"
