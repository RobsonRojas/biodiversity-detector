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
