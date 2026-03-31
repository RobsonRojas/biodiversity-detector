## Context

The system currently relies on a single-class TFLite model for chainsaw detection. The goal is to move to a multi-class model and enable audio retransmission over a bandwidth-constrained LoRa mesh network.

## Goals / Non-Goals

**Goals:**
- Implement multi-class sound classification on ESP32.
- Enable audio fragment transmission over LoRa Mesh.
- Provide a configuration toggle for audio forwarding.
- Reassemble and play back audio in the Web Manager and Telegram.

**Non-Goals:**
- Real-time high-fidelity audio streaming (LoRa is too slow).
- On-device audio playback.
- Training the TFLite model (model binary is assumed provided).

## Decisions

### 1. Multi-class TFLite Model
- **Decision**: Replace `motoserra_detect_v1.tflite` with `forest_sounds_v1.tflite`.
- **Rationale**: A single model for multiple classes is more memory-efficient than multiple specialized models on the ESP32.
- **Alternatives**: Running multiple models (too much RAM/Flash).

### 2. Audio Compression and Fragmentation
- **Decision**: Use 8kHz, 8-bit Mu-law encoding and fragment into 180-byte mesh packets.
- **Rationale**: Minimal CPU overhead for ESP32 compared to Opus, and reduces data size by 2x compared to 16-bit PCM. LoRa mesh MTU is limited.
- **Alternatives**: Opus (too complex for current firmware), raw PCM (too large for LoRa).

### 3. Mesh Protocol Extension
- **Decision**: Introduce `MeshDataPacket` for audio fragments, separate from the `MeshHeader` alert.
- **Rationale**: Keeps the heartbeats and alerts lightweight while allowing larger data transfers to span multiple packets.

### 4. Configuration Persistence
- **Decision**: Store `operation_mode` in NVS (Non-Volatile Storage) on the ESP32, synced from Supabase via the existing config mechanism.
- **Rationale**: Ensures the node remembers its state across reboots.

## Risks / Trade-offs

- **[Risk] High Airtime Overload** → **Mitigation**: Limit audio clips to 3-5 seconds and implement a "cooldown" between audio transmissions.
- **[Risk] Packet Loss in Mesh** → **Mitigation**: Use simple sequence numbers; the web manager will zero-fill missing gaps in the audio file.
- **[Risk] RAM exhaustion on ESP32** → **Mitigation**: Reuse the existing `CircularBuffer` for both inference and fragmentation.
