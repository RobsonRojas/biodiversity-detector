## 1. Environment and Project Structure

- [ ] 1.1 Create `src/esp32-s3-cpp` directory and initialize ESP-IDF project with CMake.
- [ ] 1.2 Configure `sdkconfig` for ESP32-S3 with PSRAM and low-power optimzations.
- [ ] 1.3 Add dependencies: `esp-dsp`, `esp_nn`, and Edge Impulse C++ SDK.

## 2. Audio Pipeline and DSP

- [ ] 2.1 Implement I2S microphone driver with DMA and double-buffering.
- [ ] 2.2 Implement multi-band FFT spectral analysis (100Hz to 20kHz) using `esp-dsp`.
- [ ] 2.3 Create RMS threshold logic for the ULP/RTC wake-up phase.
- [ ] 2.4 Implement frequency band detection for biological triggers (insects/amphibians).

## 3. AI Inference and LoRa

- [ ] 3.1 Integrate Edge Impulse C++ bundle and TFLite Micro runtime.
- [ ] 3.2 Implement multi-class inference wrapper (Chainsaw, Animal, Insect, Amphibian).
- [ ] 3.3 Port LoRa mesh communication logic to C++ and integrate with the alert/telemetry system.
- [ ] 3.4 Implement periodic telemetry reporting for biodiversity events.

## 4. Documentation and Architecture

- [ ] 4.1 Create `docs/architecture.md` with sequence diagrams for power cycles and data flow.
- [ ] 4.2 Configure Doxygen for API documentation generation.
- [ ] 4.3 Develop `examples/` directory with standalone modules for I2S, DSP, and AI.
- [ ] 4.4 Create `README.md` and `DEVELOPMENT.md` in the firmware directory with full integration examples.
