## 1. Documentation Infrastructure

- [ ] 1.1 Create the directory `docs/architecture/` and a root `README.md`.
- [ ] 1.2 Define the architectural taxonomy (C4 model explanation and Mermaid conventions).

## 2. C4 Model - High Level

- [ ] 2.1 Create **Level 1 (System Context)** diagram showing the Guardian nodes in the forest context.
- [ ] 2.2 Create **Level 2 (Container)** diagram highlighting the boundaries between physical hardware, LoRa mesh, and cloud backends.

## 3. C4 Model - Component Level

- [ ] 3.1 Create **Level 3 (Component)** diagram for the Firmware container, detailing Audio, AI, and Telemetry modules.
- [ ] 3.2 Detail the **AlertManager** component as the central hub for event orchestration.

## 4. Detailed UML and Flow

- [ ] 4.1 Create a **Class Diagram** for the core C++ classes (`AlertManager`, `DetectionEngine`, `AudioIn`, `LoRaDriver`).
- [ ] 4.2 Create a **Sequence Diagram** for the "Positive Detection" flow (from I2S capture to Supabase/Telegram notification).
- [ ] 4.3 Develop a **State Machine** diagram for the 3-stage power cascade (Quiet, Transition, Active states).
