# Firmware Component Architecture

This document covers Level 3 (Component) of the system architecture, focusing on the ESP32-S3 Firmware.

## Level 3: ESP32-S3 Firmware Components

The firmware is organized into modular components that interact to achieve low-power chainsaw detection.

```mermaid
C4Component
    title Component Diagram for ESP32 Firmware
    
    Component(audio, "Audio Component", "I2S DMA", "Captures audio samples and manages double buffers.")
    Component(dsp, "DSP Module", "esp-dsp", "Performs spectral analysis and pattern extraction.")
    Component(ai, "AI Engine", "TFLite Micro", "Executes the sound classification model.")
    
    Component(alert, "Alert Manager", "C++ Class", "Orchestrates detections and determines reporting logic.")
    
    Component(telemetry, "Telemetry Module", "C++ Lib", "Handles Supabase/Telegram clients and Offline Queue.")
    Component(mesh, "Mesh Protocol", "Custom", "Manages hopping, routing, and packet serialization.")

    Rel(audio, dsp, "Feeds raw PCM", "Pointer")
    Rel(dsp, ai, "Triggers Inference", "Event")
    Rel(ai, alert, "Reports Class + Confidence", "Callback")
    
    Rel(alert, mesh, "Queues Mesh Alert", "Struct")
    Rel(alert, telemetry, "Direct Cloud sync (if Gateway)", "HTTPS")
    
    Rel(mesh, mesh, "Relays other nodes' packets", "LoRa")
```

## Core Component: Alert Manager

The `AlertManager` is the central orchestrator of the firmware. It fulfills three critical roles:

1.  **Deduplication**: Ensures that transient or low-confidence detections do not flood the network.
2.  **Role-Based Routing**: 
    - **Node Role**: Packages detections into `MeshPacket` and sends to the nearest router.
    - **Gateway Role**: Unpacks `MeshPacket`, reassembles audio fragments, and syncs to Supabase/Telegram.
3.  **Persistence**: Manages the `OfflineQueue` for detections made during network outages.

### Data Reassembly Logic (Gateway)
When audio fragments arrive via the mesh, the AlertManager manages `AudioSession` states using a map of fragment indices.

```mermaid
graph TD
    P[Mesh Packet] --> V{Is Audio Fragment?}
    V -->|Yes| S[Retrieve/Create AudioSession]
    S --> L[Store fragment in map]
    L --> C{All Fragments Received?}
    C -->|Yes| F[Reassemble & Upload to Supabase Storage]
    C -->|No| R[Wait for next fragment]
    V -->|No| H[Standard Telemetry Handling]
```
