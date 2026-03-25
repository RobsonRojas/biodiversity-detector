#!/bin/bash
# scripts/test-integration.sh

set -e

echo "============================================================"
# echo "Starting Motoserra Detect System Integration Tests"
echo "============================================================"

# 1. Prepare Mock Data
echo "[1/4] Generating mock audio data..."
python3 scripts/mock_sensor.py motoserra /tmp/motoserra.bin
python3 scripts/mock_sensor.py background /tmp/background.bin

# 2. Build the Application (if not already built by Docker)
# In this environment, we expect the app to be in /app/build/guardian_node
APP_PATH="/app/build/guardian_node"

if [ ! -f "$APP_PATH" ]; then
    echo "ERROR: Application not found at $APP_PATH. Build it first."
    exit 1
fi

# 3. Execution - Test Case: Motoserra Detection
echo "[2/4] Running detection test (Motoserra)..."
# Use qemu-aarch64-static if running aarch64 on x86_64
QEMU_WRAPPER=""
if [ "$(uname -m)" != "aarch64" ]; then
    QEMU_WRAPPER="qemu-aarch64-static -L /usr/aarch64-linux-gnu/"
fi

# We run the app for 15 seconds (enough for one 5s active phase + buffer fill)
set +e
timeout --preserve-status 15s $QEMU_WRAPPER "$APP_PATH" /tmp/motoserra.bin > /tmp/test_log.txt 2>&1
RET=$?
set -e

echo "[3/4] Analyzing logs..."
cat /tmp/test_log.txt

if grep -q "Motoserra detected!" /tmp/test_log.txt; then
    echo "SUCCESS: Motoserra correctly detected in audio stream."
else
    echo "FAILURE: Motoserra was NOT detected in audio stream."
    exit 1
fi

echo "[4/4] Running false-positive test (Background)..."
set +e
timeout --preserve-status 15s $QEMU_WRAPPER "$APP_PATH" /tmp/background.bin > /tmp/test_log_bg.txt 2>&1
RET_BG=$?
set -e

echo "[DEBUG] Background logs:"
cat /tmp/test_log_bg.txt

if grep -q "Motoserra detected!" /tmp/test_log_bg.txt; then
    echo "FAILURE: False positive detected in background noise!"
    exit 1
else
    echo "SUCCESS: No false alerts in background noise."
fi

echo "============================================================"
echo "Integration Tests Passed Successfully!"
echo "============================================================"
exit 0
