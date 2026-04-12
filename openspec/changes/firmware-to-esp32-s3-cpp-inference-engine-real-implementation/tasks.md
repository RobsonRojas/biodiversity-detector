## 1. SDK Integration & Configuration

- [x] 1.1 Add Edge Impulse SDK headers and libraries to `src/esp32-s3-cpp/main/CMakeLists.txt`
- [x] 1.2 Verify that the required model files (e.g., `model_metadata.h`, `tflite-model/`) are accessible in the build path

## 2. Core Implementation

- [x] 2.1 Refactor `InferenceEngine::init()` to initialize the Edge Impulse interpreter and allocate the tensor arena
- [x] 2.2 Implement `InferenceEngine::classify()` to execute the TFLite model on normalized audio features
- [x] 2.3 Add error handling for initialization failures or empty feature buffers

## 3. Pipeline & Telemetry

- [x] 3.1 Connect the `DspAnalyzer` output to the `InferenceEngine::classify` call in `main.cpp`
- [x] 3.2 Ensure classification results are formatted and logged via `ESP_LOGI` for serial telemetry
- [x] 3.3 Verify that the inference task respects the audio capture priority to avoid buffer overruns

## 4. Verification & Clean-up

- [x] 4.1 Verify that PSRAM is correctly utilized for the large tensor arena
- [x] 4.2 Test the end-to-end flow from audio capture to result logging
- [x] 4.3 Remove any remaining mock simulation logic from `inference_engine.cpp`
