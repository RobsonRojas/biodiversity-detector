#!/bin/bash
set -e

# GuardianCore ESP32-S3 Firmware Build Script

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
DIST_DIR="$PROJECT_ROOT/dist"
ESP32_DIR="$PROJECT_ROOT/src/esp32"

mkdir -p "$DIST_DIR"

echo "Checking for ESP32 Rust toolchain..."
if ! command -v cargo &> /dev/null; then
    echo "ERROR: cargo not found. Please install Rust."
    exit 1
fi

# Check for Xtensa toolchain
if ! rustup target list | grep -q "xtensa-esp32s3-espidf"; then
    echo "WARNING: Xtensa target not found in rustup. Attempting to build using Docker..."
    if command -v docker &> /dev/null; then
        echo "Running Docker build (Stage: builder-esp32)..."
        docker build --target builder-esp32 -t esp32-builder "$PROJECT_ROOT"
        docker run --rm esp32-builder cat /app/src/esp32/target/xtensa-esp32s3-espidf/release/esp32 > "$DIST_DIR/esp32.bin"
        echo "Firmware extracted to $DIST_DIR/esp32.bin"
    else
        echo "ERROR: Docker not found. Cannot cross-compile for ESP32-S3."
        exit 1
    fi
else
    echo "Building ESP32 firmware locally..."
    cd "$ESP32_DIR"
    cargo build --target xtensa-esp32s3-espidf --release
    cp "target/xtensa-esp32s3-espidf/release/esp32" "$DIST_DIR/esp32.bin"
    echo "Build successful: $DIST_DIR/esp32.bin"
fi
