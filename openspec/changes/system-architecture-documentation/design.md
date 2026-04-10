## Context

The project lacks a unified architectural map. With the transition to a multi-stage triggered C++ firmware on ESP32-S3 and complex LoRa mesh interactions, visual documentation is necessary to maintain system integrity.

## Goals / Non-Goals

**Goals:**
- Provide a clear, visual representation of the system using C4 and UML.
- Document all core modules: Audio, AI, HAL, and Telemetry.
- Map the power management states across the system.

**Non-Goals:**
- Documentation of experimental/archived code (e.g., legacy Rust code unless it's currently active).
- Generating code from diagrams (MDD).

## Decisions

### 1. Mermaid.js for Diagrams
Diagrams will be implemented directly in Markdown files using Mermaid blocks. This ensures diagrams are tracked in version control alongside the documentation.

### 2. C4 Model Layers
We will implement the first three layers of the C4 model:
- **Level 1 (Context)**: The Guardian System in its environment (Forest -> Mesh -> Backend).
- **Level 2 (Container)**: The ESP32-S3 Firmware as a container, interacting with the LoRa Driver and Backend API.
- **Level 3 (Component)**: Internal structure of the firmware (AlertManager, DetectionEngine, AudioIn, etc.).

### 3. State-Flow Diagrams
Special focus will be given to the **3-Stage Cascade** (Quiet -> DSP -> Inference). This will be documented using a State Machine diagram to clarify wake-up and sleep transitions.

### 4. Organization
Architecture documentation will be stored in `docs/architecture/` with a root `README.md` serving as the entry point.

## Risks / Trade-offs

- **Risk: Diagram Stale-ness.** Code changes frequently, making diagrams obsolete.
    - **Mitigation**: Architecture documentation is a required artifact for any major architectural change.
- **Trade-off: Mermaid vs. Graphic Tools.** Mermaid is more limited in layout control but significantly easier to maintain in a git-based workflow.
