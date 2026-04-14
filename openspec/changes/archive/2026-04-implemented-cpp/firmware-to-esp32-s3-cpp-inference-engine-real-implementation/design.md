## Context

The project currently uses a mock `InferenceEngine` that hardcodes classification results. We have an Edge Impulse model trained for chainsaw detection and biodiversity monitoring (animals, insects) that must be integrated. The firmware already has a robust I2S capture and DSP pipeline delivering normalized float samples, which are the required input for the inference engine.

## Goals / Non-Goals

**Goals:**
- Replace mock classification logic with the actual Edge Impulse / TensorFlow Lite Micro SDK.
- Support multi-class detection (Chainsaw, Animal, Insect, Background).
- Optimize for ESP32-S3 by utilizing PSRAM for the tensor arena if needed.
- Maintain real-time performance to prevent audio buffer overruns.

**Non-Goals:**
- Model retraining or dataset modification.
- Changes to the LoRa or Telegram communication protocols.

## Decisions

- **Inference Library**: Use the Edge Impulse C++ SDK (which wraps TFLite Micro) for its native ESP-NN optimizations on the S3.
- **Memory Management**: Allocate the model's tensor arena in PSRAM to prevent internal SRAM exhaustion, while keeping critical SDK overhead in SRAM for speed.
- **Initialization Strategy**: Initialize the model once during `InferenceEngine::init()` to minimize latency during the first classification.
- **Output Mapping**: Map the raw probability array from the model to the `std::string& result_label` and `float& confidence` expected by the existing system.

## Risks / Trade-offs

- **Performance vs. Precision**: Using PSRAM for the tensor arena may introduce slight latency compared to SRAM, but it is necessary for the model's memory requirements.
- **Interrupt Handling**: Inference must run in a low-priority task (or the main loop) to avoid jitter in the high-fidelity audio capture pipeline.
