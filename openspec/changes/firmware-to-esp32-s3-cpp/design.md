## Context

The current Rust implementation is being migrated to C++ to leverage the more mature ESP-IDF ecosystem for high-performance DSP (esp-dsp) and Edge Impulse integration. The system must maintain a high level of battery efficiency while providing robust chainsaw detection.

## Goals / Non-Goals

**Goals:**
- Implement a C++ firmware using ESP-IDF v5.x.
- 3-stage power cascade: Quiet (ULP monitoring), Transition (DSP spectral analysis), Active (CNN Inference).
- Comprehensive architectural documentation with code examples.
- Integration with existing LoRa mesh network.

**Non-Goals:**
- Training the AI model (we use pre-trained Edge Impulse bundles).
- Complete replacement of the Rust codebase (C++ will co-exist or focus on the S3 target).

## Decisions

### 1. ESP-IDF C++ Framework
We will use the native ESP-IDF C++ components. This allows direct integration with `esp-dsp` for spectral analysis and `esp-nn` for optimized tensor operations.

### 2. Documentation Architecture
Documentation will be split into three tiers:
- **High-Level**: Markdown files in `docs/` explaining the system architecture and power cycles.
- **API Reference**: Doxygen-style header comments for all internal libraries.
- **Implementation Guide**: A specific `DEVELOPMENT.md` with walkthroughs and code snippets for common tasks (e.g., adding a new alert type).

### 3. Audio Pipeline
- **I2S DMA**: Audio will be captured via I2S using DMA into dual buffers to prevent dropouts during DSP/AI processing.
- **DSP Filter**: A 256-point FFT will be used to check for energy in the 100Hz-2kHz range before waking the AI task.

### 4. Code Examples Module
A set of standalone ESP-IDF components will be created in `examples/` that demonstrate minimal usage of I2S, LoRa, and AI Inference, serving as both documentation and test harnesses.

## Risks / Trade-offs

- **Risk: Complexity of documentation maintenance.** Keeping documentation in sync with C++ code can be difficult.
    - **Mitigation**: Use Doxygen to auto-generate API documentation from code comments.
- **Trade-off: Memory vs. Power.** Larger circular buffers for audio improve detection reliability but increase SRAM/power usage.
