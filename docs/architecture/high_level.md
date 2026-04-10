# High-Level System Architecture

This document covers Level 1 (Context) and Level 2 (Container) of the Guardian system architecture.

## Level 1: System Context

The Guardian system interacts with biological researchers (users), the physical environment (audio source), and various cloud alerting services.

```mermaid
C4Context
    title System Context Diagram for Guardian Detector
    
    Person(user, "Bio-Researcher", "Monitors forest health and illegal activity.")
    System(guardian, "Guardian Detector System", "Detects chainsaws and monitors biodiversity in real-time.")
    
    System_Ext(env, "Forest Environment", "Source of acoustic signals (Chainsaws, Animals, Weather).")
    System_Ext(telegram, "Telegram/Supabase", "Receives alerts and telemetry data.")
    System_Ext(mesh_ext, "Remote Nodes", "Other detectors in the mesh network.")

    Rel(env, guardian, "Acoustic Signals", "Audio/I2S")
    Rel(guardian, telegram, "Sends alerts and audio samples", "HTTPS/JSON")
    Rel(guardian, mesh_ext, "Relays mesh packets", "LoRa/Proprietary")
    Rel(user, telegram, "Views data and reports", "Mobile/Web")
```

## Level 2: Container

Within the Guardian system, we distinguish between the physical sensor node, the mesh network layer, and the cloud infrastructure.

```mermaid
C4Container
    title Container Diagram for Guardian node
    
    Container(firmware, "ESP32-S3 Firmware", "C++/ESP-IDF", "Core detection and communication logic.")
    ContainerDb(rtc, "RTC Memory", "SRAM", "Persistent store for logs during deep sleep.")
    
    Container(lora, "LoRa Radio", "SX1262", "Physical layer for mesh communication.")
    
    System_Boundary(c1, "Cloud Backend") {
        Container(supabase, "Supabase Database", "PostgreSQL", "Stores detection events and device metadata.")
        Container(storage, "Audio Storage", "S3/Bucket", "Stores archived audio fragments.")
        Container(telegram_bot, "Alerter Bot", "Node.js", "Propagates alerts to users.")
    }

    Rel(firmware, rtc, "Stores/Retrieves state", "SRAM Access")
    Rel(firmware, lora, "Sends/Receives packets", "SPI/GPIO")
    Rel(lora, lora, "Mesh Traffic", "LoRa Waves")
    
    Rel(firmware, supabase, "Syncs Telemetry (via Gateway)", "REST/JSON")
    Rel(firmware, storage, "Uploads Audio (via Gateway)", "REST/Binary")
    Rel(firmware, telegram_bot, "Triggers Alerts (via Gateway)", "Webhooks")
```
