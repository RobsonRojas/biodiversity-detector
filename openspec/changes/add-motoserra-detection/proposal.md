## Why

The Amazon Rainforest is under constant threat from illegal logging. Detecting the sound of chainsaws (motoserras) in real-time is the most effective way to trigger rapid response and protect the ecosystem. This change implements the core acoustic detection capability for the Patronos-Forest-Guardian system, enabling the edge device to monitor and flag illegal activities while operating under extreme power constraints.

## What Changes

1.  **AI Detection Engine**: Implementation of a TFLite Micro inference engine optimized for `int8` quantization.
2.  **Audio Ingestion Logic**: Integration with the I2S microphone driver to process 16kHz audio streams.
3.  **Buffer Management**: Implementation of a sliding window buffer to provide 1-second audio frames to the AI model.
4.  **Confidence Thresholding**: Logic to trigger telemetry alerts only when detection confidence exceeds 0.85.

## Capabilities

### New Capabilities
- `motoserra-detection`: Real-time acoustic classification of chainsaw sounds using TFLite Micro on the edge.
- `audio-stream-processing`: High-efficiency ingestion of I2S audio data into processing buffers.

### Modified Capabilities
- (None)

## Impact

- **AI Module**: Primary implementation of the `tflite_micro` framework.
- **Hardware Integration**: Heavy dependency on the Rust-based I2S driver (`rust_kernel_module_v1`).
- **Power Profile**: The detection loop must fit within the 500mW power budget defined in `config.yaml`.
