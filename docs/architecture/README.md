# Guardian System Architecture Documentation

Welcome to the architectural specification for the Guardian Chainsaw Detection System. This repository provides a multi-level view of the system's design, from high-level ecosystem interactions to low-level class hierarchies.

## Architectural Methodology: The C4 Model

We use the [C4 model](https://c4model.com/) to document our architecture across different levels of abstraction:

1.  **Level 1: System Context** ([high_level.md](high_level.md#level-1-system-context))
    - Shows the Guardian system as a whole and its interactions with the environment (forest, mesh, backends).
2.  **Level 2: Container** ([high_level.md](high_level.md#level-2-container))
    - Refines the system into containers like the ESP32-S3 Firmware, the LoRa Mesh Network, and the Cloud Services.
3.  **Level 3: Component** ([firmware_components.md](firmware_components.md))
    - Decomposes the firmware container into its internal modules (Audio, AI, Telemetry).

## Technical Deep-Dives

For developers looking for implementation details, we provide:
- **Class Diagrams**: Structural relationships in the C++ codebase.
- **Sequence Diagrams**: Dynamic flows of detections and mesh communication.
- **State Machines**: Power management cycles (3-Stage Cascade).

Refer to [technical_details.md](technical_details.md) for these diagrams.

## Diagram Conventions

All diagrams are implemented using **Mermaid.js** syntax within the Markdown files.
- **Blue Boxes**: Internal Guardian components.
- **Gray Boxes**: External systems or entities.
- **Solid Lines**: Synchronous calls or direct hardware interaction.
- **Dashed Lines**: Asynchronous messages or mesh propagations.
