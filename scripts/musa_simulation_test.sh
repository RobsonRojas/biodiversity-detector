#!/bin/bash
# MUSA Forest Simulation Orchestrator
# Triggers detection events and monitors multi-hop propagation

NODE1_ID="node1"
GATEWAY_ID="node5"
TRIGGER_CHAINSAW="/tmp/chainsaw_trigger"
TRIGGER_FROG="/tmp/frog_trigger"

echo "=== MUSA Forest Simulation Orchestrator ==="
echo "Location: Jardim Botânico de Manaus (-3.0016, -59.9405)"

# 1. Wait for mesh network to stabilize
echo "[STEP 1] Waiting for mesh convergence (30s)..."
sleep 30

# 2. Trigger Chainsaw Detection
echo "[STEP 2] Triggering Chainsaw Detection on Node 1..."
docker exec $NODE1_ID touch $TRIGGER_CHAINSAW
echo "Waiting 10s for propagation..."
sleep 10

# 3. Trigger Frog Detection (Boana geographica)
echo "[STEP 3] Triggering Frog (Boana geographica) Detection on Node 1..."
docker exec $NODE1_ID touch $TRIGGER_FROG
echo "Waiting 10s for propagation..."
sleep 10

# 4. Verify Gateway Reception
echo "[STEP 4] Checking Gateway (Node 5) logs for received alerts..."
docker logs $GATEWAY_ID | grep -E "Detection|Alert|Boana"

echo "=== Simulation Triggers Completed ==="
