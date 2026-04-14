## Context

The system currently lacks a power-aware detection strategy for ESP32-S3 hardware. Deployment in forest environments requires extreme battery efficiency. This design proposes a cascaded detection architecture that minimizes the "on-time" of the main AI core.

## Goals / Non-Goals

**Goals:**
- Implement a 3-stage power cascade: Quiet (Deep Sleep), Transition (DSP), Active (Inference).
- Achieve sub-1mA average current draw in monitoring mode.
- Support multi-class sound classification including chainsaws and environmental sounds.

**Non-Goals:**
- Real-time audio streaming (telemetry only).
- Training the model (this change covers model *integration*).

## Decisions

### 1. Cascaded Triggering
We use a "Leaky Bucket" RMS threshold in the ULP/RTC controller for initial wake-up. This avoids waking the main CPUs for transient noise.

### 2. ESP-DSP for Pattern Matching
Before running inference, we use the `esp-dsp` library to perform a quick Spectral Centroid and Bandwidth analysis. If the spectral signature doesn't match the harmonic profile of a motor (chainsaw), we abort the inference cycle immediately.

### 3. Edge Impulse / TFLite Micro Integration
We will use the C++ deployment bundle from Edge Impulse. This is chosen for its superior I2S integration and optimized CMSIS-NN kernels for the ESP32-S3's vector instructions.

## Risks / Trade-offs

- **Risk: Late Activation.** If the RMS-to-DSP transition is too slow, we might miss the beginning of a short sound event.
    - **Mitigation:** Use a small circular buffer in RTC memory if the microphone supports I2S DMA during light sleep.
- **Trade-off: Accuracy vs. Power.** The DSP filter might reject some edge cases that the AI would correctly classify, but this is acceptable to achieve the power goals.
