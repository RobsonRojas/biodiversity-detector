# Technical Architecture Details

This document provides low-level technical diagrams for the Guardian system implementation.

## 1. Class Diagram

Relationships between the core C++ classes in the `src/` directory.

```mermaid
classDiagram
    class AlertManager {
        -NodeConfig config
        -shared_ptr~TelegramClient~ client
        -shared_ptr~OfflineQueue~ queue
        -shared_ptr~LoRaDriver~ lora
        -shared_ptr~SupabaseClient~ supabase
        +on_detection(event)
        +on_mesh_receive(data)
        +send_heartbeat(bat, rssi)
    }

    class DetectionEngine {
        +initialize(model_path)
        +run_inference(buffer)
    }

    class AudioIn {
        -string device_path
        +open()
        +read_int16(buffer)
    }

    class LoRaDriver {
        <<interface>>
        +initialize()*
        +send(data)*
        +receive(span)*
    }

    class PhysicalLoRaDriver {
        +initialize()
    }

    class SimulatedLoRaDriver {
        +initialize()
    }

    AlertManager --> LoRaDriver : uses
    AlertManager --> TelegramClient : uses
    AlertManager --> SupabaseClient : uses
    LoRaDriver <|-- PhysicalLoRaDriver : implements
    LoRaDriver <|-- SimulatedLoRaDriver : implements
```

## 2. Sequence Diagram: Positive Detection Flow

The flow of data from physical sensor capture to cloud notification.

```mermaid
sequenceDiagram
    participant Mic as AudioIn (I2S)
    participant DSP as CascadeFilter
    participant AI as DetectionEngine
    participant AM as AlertManager
    participant Mesh as LoRaMesh
    participant Cloud as Supabase/Telegram

    Mic->>DSP: Capture Raw PCM
    DSP->>DSP: Check RMS & FFT
    Note over DSP: Match Found
    DSP->>AI: Trigger Inference
    AI->>AM: Detection Event (Label, Conf)
    
    alt Node Role
        AM->>Mesh: Package as MeshPacket
        Mesh->>Mesh: Relay to Gateway
    else Gateway Role
        AM->>Cloud: POST /detections
        Cloud-->>AM: 200 OK
    end
```

## 3. State Machine: 3-Stage Power Cascade

Details the transitions between low-power states on the ESP32-S3.

```mermaid
stateDiagram-v2
    [*] --> Quiet : Reset
    
    state Quiet {
        [*] --> DeepSleep
        DeepSleep --> RTC_Check : 100ms interval
        RTC_Check --> DeepSleep : RMS < Threshold
    }

    Quiet --> Transition : RMS > Threshold
    
    state Transition {
        [*] --> LightSleep
        LightSleep --> DSP_FFT : Wake main CPU
        DSP_FFT --> LightSleep : No Pattern Match
    }

    Transition --> Active : Pattern Match Result
    
    state Active {
        [*] --> FullPower
        FullPower --> Inference : Run TFLite
        Inference --> Report : Confidence > 0.8
        Inference --> FullPower : Continuous Monitoring
    }

    Active --> Quiet : No triggers for 10s
    Transition --> Quiet : No triggers for 10s
```

## 4. Geospatial Localization Propagation

The system uses a recursive propagation model to geolocate nodes without individual GPS modules.

### Node Localization: Iterative Multi-lateration (IML)
1. **Gateway Anchor**: The gateway (physical anchor) broadcasts its precise (lat, lon) with 1m accuracy.
2. **Distance Estimation**: 1-hop nodes estimate distance from the anchor using the **Log-Distance Path Loss Model** based on RSSI.
3. **Recursive Calculation**: Once a node has 3+ neighbor anchors, it running a Least Squares solver (`localization_module.cpp`) to calculate its coordinates and then begins broadcasting as a "virtual anchor".

### Acoustic Event Localization: Weighted Centroid (WCL)
1. **Correlation**: The Supabase `ingest-mesh-data` function groups detections from multiple nodes within a 10s window.
2. **Triangulation**: The source coordinates $(x, y)$ are calculated as a weighted average of the reporting nodes' positions, weighted by detection confidence.

```mermaid
graph TD
    GW[Gateway / GPS Anchor] -->|Beacon| N1[Node 1: 1-Hop]
    GW -->|Beacon| N2[Node 2: 1-Hop]
    N1 -->|RSSI Estimate| N3[Node 3: 2-Hop]
    N2 -->|RSSI Estimate| N3
    N1 -->|Beacon| N3
    N3 -->|3 Anchors Reached| N3Pos[Calculate (x,y) via IML]
    
    Sound((Chainsaw)) -.-> N1
    Sound -.-> N3
    N1 -->|Detection (lat1, lon1)| Cloud[Supabase Cloud]
    N3 -->|Detection (lat3, lon3)| Cloud
    Cloud -->|WCL| Event[(Acoustic Event Map)]
```
