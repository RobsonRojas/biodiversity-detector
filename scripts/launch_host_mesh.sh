#!/bin/bash
# Host-Only 5-Node Mesh Simulation (MUSA Forest)

BINARY="./build/guardian_node"
LOG_DIR="./sim_logs"
mkdir -p $LOG_DIR

# Kill existing
pkill -f guardian_node

echo "=== Launching 5-Node Host Mesh (MUSA Forest) ==="

# Node 5: Gateway (MUSA Tower)
echo "Starting Node 5 (Gateway)..."
USE_SIM_LORA=1 SIM_LORA_PORT=5005 NODE_ID=0x0005 NODE_ROLE=Gateway SIM_LAT=-3.0016 SIM_LON=-59.9405 SIM_DISABLE_SLEEP=1 $BINARY > $LOG_DIR/node5.log 2>&1 &
sleep 1

# Node 4: Router
echo "Starting Node 4 (Router)..."
USE_SIM_LORA=1 SIM_LORA_PORT=5004 NODE_ID=0x0004 NODE_ROLE=Router SIM_LAT=-3.0036 SIM_LON=-59.9425 SIM_DISABLE_SLEEP=1 SIM_LORA_NEIGHBORS=localhost:5005 MESH_ROUTES=0x0005:0x0005 $BINARY > $LOG_DIR/node4.log 2>&1 &
sleep 1

# Node 3: Router
echo "Starting Node 3 (Router)..."
USE_SIM_LORA=1 SIM_LORA_PORT=5003 NODE_ID=0x0003 NODE_ROLE=Router SIM_LAT=-3.0056 SIM_LON=-59.9445 SIM_DISABLE_SLEEP=1 SIM_LORA_NEIGHBORS=localhost:5004 MESH_ROUTES=0x0005:0x0004 $BINARY > $LOG_DIR/node3.log 2>&1 &
sleep 1

# Node 2: Router
echo "Starting Node 2 (Router)..."
USE_SIM_LORA=1 SIM_LORA_PORT=5002 NODE_ID=0x0002 NODE_ROLE=Router SIM_LAT=-3.0076 SIM_LON=-59.9465 SIM_DISABLE_SLEEP=1 SIM_LORA_NEIGHBORS=localhost:5003 MESH_ROUTES=0x0005:0x0003 $BINARY > $LOG_DIR/node2.log 2>&1 &
sleep 1

# Node 1: Leaf (Detector - Jardim Botânico)
echo "Starting Node 1 (Leaf)..."
USE_SIM_LORA=1 SIM_LORA_PORT=5001 NODE_ID=0x0001 NODE_ROLE=Leaf SIM_LAT=-3.0096 SIM_LON=-59.9485 SIM_DISABLE_SLEEP=1 SIM_LORA_NEIGHBORS=localhost:5002 MESH_ROUTES=0x0005:0x0002 $BINARY > $LOG_DIR/node1.log 2>&1 &

echo "Mesh Network Up. Monitoring Gateway logs..."
tail -f $LOG_DIR/node5.log
