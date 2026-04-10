## Why

As the project grows in complexity—transitioning from Rust to C++ and integrating advanced features like 3-stage power cascading, LoRa mesh networking, and AI inference—there is a critical need for a centralized, comprehensive architectural documentation. 

Current technical details are scattered across implementation files and specific change designs. A unified architecture document using standardized UML and C4 modeling will:
- Facilitate onboarding for new contributors.
- Provide a clear map of system dependencies and data flows.
- Help identify architectural bottlenecks or potential refactoring needs.
- Serve as a "source of truth" for system design beyond the current implementation.

## What Changes

1.  **Architecture Core**: Develop a comprehensive system architecture document covering all modules: Audio, AI, Telemetry, HAL, and Alert Management.
2.  **UML Integration**: Use Mermaid diagrams within Markdown to implement:
    - **C4 Model**: Context, Container, and Component diagrams.
    - **Component Diagrams**: High-level module interactions.
    - **Class Diagrams**: Inheritance and dependency relations for core managers (AlertManager, DetectionEngine, etc.).
    - **Call Graphs/Sequence Diagrams**: Visualizing the detection-to-alert flow and mesh communication cycles.
3.  **Cross-cutting Documentation**: Document power management states and their transitions across modules.

## Capabilities

### New Capabilities
- `system-architecture-docs`: A new, living documentation repository for the entire system architecture, using UML and C4 models.

### Modified Capabilities
- `esp32-s3-cpp-firmware`: Will now reference the centralized architecture for its implementation details.
- `firmware-architecture-docs`: Will be integrated into or reference the main system architecture documentation.

## Impact

- **Documentation Overhaul**: New files in `docs/architecture/` or integrated into `openspec`.
- **Visibility**: Improved understanding of the system's "big picture" for all stakeholders.
- **Maintainability**: Clearer boundaries between modules (Audio, AI, LoRa) leading to better decoupling.
