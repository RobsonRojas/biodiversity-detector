# esp32-s3-cpp-firmware Specification

## Purpose
Define the high-performance, low-power acoustic detection firmware for the ESP32-S3 using the ESP-IDF framework. It implements a 3-stage cascaded detection pipeline to maximize battery life while maintaining high accuracy for chainsaw and biodiversity sounds.

## Requirements

### Requirement: 3-stage-cascade-detection
The system MUST implement a cascaded processing pipeline to reduce power consumption.

#### Scenario: RMS Wake-on-Sound (Stage 1)
- **WHEN** in low-power monitoring mode
- **THEN** calculate the RMS of the incoming audio buffer (44.1kHz, 32-bit MONO)
- **IF** RMS > threshold (defined in `config.yaml`)
- **THEN** promote to Stage 2.

#### Scenario: DSP Pattern Verification (Stage 2)
- **WHEN** sound is detected by RMS
- **THEN** perform FFT and analyze energy in mechanical (chainsaw) vs biological (wildlife) frequency bands.
- **IF** energy pattern suggests an event of interest
- **THEN** wake up the AI Inference Engine (Stage 3).

#### Scenario: AI Inference (Stage 3)
- **WHEN** DSP verification is positive
- **THEN** collect a 2-second window of audio (88200 samples)
- **THEN** execute the TFLite Micro model (quantized `int8`)
- **IF** classification confidence > 0.8
- **THEN** generate an alert and transmit via `lora_mesh`.

### Requirement: zero-copy-audio-buffer
The firmware MUST use a lock-free `SharedRingBuffer` for transferring raw audio data from the DMA/I2S interrupt to the analysis pipeline.

#### Scenario: memory-safety
- **GIVEN** a buffer allocation in PSRAM
- **THEN** use `std::unique_ptr` and modern C++ memory management to avoid raw pointer leaks.

### Requirement: node-localization
The firmware MUST maintain and propagate its own geographical coordinates (Lat/Lon) derived from the mesh gateway or a fixed reference.

#### Scenario: alert-metadata
- **WHEN** an alert is generated
- **THEN** include the current `NodeCoords` in the telemetry payload.
