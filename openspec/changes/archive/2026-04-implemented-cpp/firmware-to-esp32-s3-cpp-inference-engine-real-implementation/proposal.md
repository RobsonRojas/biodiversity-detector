## Why

The current inference engine is a mock implementation returning hardcoded values. To detect chainsaws and monitor biodiversity in real-time on the ESP32-S3 hardware, we need to integrate the actual Edge Impulse / TensorFlow Lite Micro SDK. This will provide high-performance, low-power inference capabilities required for autonomous field deployment.

## What Changes

- Transition `InferenceEngine` from a mock to a production-ready C++ implementation.
- Integrate the Edge Impulse C++ SDK/Library into the `main` component.
- Implement memory-efficient processing of normalized features from the DSP pipeline.
- Support multi-class classification (Chainsaw, Animal, Insect, Background) based on the Edge Impulse model.

## Capabilities

### New Capabilities
- `edge-impulse-inference`: Native C++ implementation of the Edge Impulse inference SDK for real-time AI.
- `ai-prediction-pipeline`: Full data flow from I2S capture -> DSP preprocessing -> AI classification.

### Modified Capabilities
- `environmental-monitoring`: Enabling authentic biodiversity and threat detection data.

## Impact

- `src/esp32-s3-cpp/main/inference_engine.cpp`: Real logic for model initialization and execution.
- `src/esp32-s3-cpp/main/CMakeLists.txt`: Linking with Edge Impulse SDK headers and libraries.
- `src/esp32-s3-cpp/main/main.cpp`: Proper handling of classification results for telemetry.
