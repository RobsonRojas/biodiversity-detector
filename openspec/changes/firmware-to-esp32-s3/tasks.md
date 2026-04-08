## 1. Power Management & Hardware Setup

- [x] 1.1 Configure ESP32-S3 RTC peripherals for Deep Sleep.
- [x] 1.2 Implement Wake-on-GPIO interrupt logic for analog MEMS trigger.
- [x] 1.3 Initialize I2S peripheral for DMA-based audio capture upon wake-up.

## 2. Audio Processing Pipeline (Cascade)

- [x] 2.1 Develop Stage 1: RMS-to-Threshold logic in RTC memory.
- [x] 2.2 Develop Stage 2: DSP Frequency band filtering using `esp-dsp` library.
- [x] 2.3 Implement transition state machine (Sleep -> Filter -> AI).

## 3. TinyML Integration

- [x] 3.1 Import Edge Impulse C++ SDK into the `src/ai` directory.
- [x] 3.2 Implement Spectrogram/Log-Mel feature extraction (Signal Processing).
- [x] 3.3 Create classification wrapper for multi-class detection (Chainsaw, Bird, etc.).

## 4. System Validation & Telemetry

- [x] 4.1 Log detection results (classification + confidence) to RTC memory.
- [x] 4.2 Format and queue alert packets for LoRaWAN transmission (AlertManager integration).
- [x] 4.3 Benchmark power consumption in each of the 3 cascade stages.
