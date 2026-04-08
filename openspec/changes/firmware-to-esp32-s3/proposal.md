## Why

The current system needs to operate in remote environments with limited power (battery/solar). Continuous high-performance audio processing and AI inference are too power-heavy for long-term deployment. By implementing a cascaded "Wake-on-Sound" strategy, the ESP32-S3 can remain in Deep Sleep most of the time, only consuming significant power when a potential sound event of interest is occurring.

## What Changes

This change implements a multi-stage detection firmware for the ESP32-S3:
1. **Low Power Stage**: Configures the ESP32-S3 for Deep Sleep with an ULP (Ultra Low Power) co-processor or a simple hardware interrupt from a MEMS microphone (RMS/threshold based).
2. **DSP Stage**: Upon waking, a basic DSP filter identifies frequency patterns characteristic of the target sounds (e.g., the specific drone/harmonic pattern of a chainsaw).
3. **TinyML Stage**: If the DSP filter validates the signal, the main AI core runs a TinyML inference (TensorFlow Lite for Microcontrollers or Edge Impulse) to classify the sound (chainsaw, bird, animal, etc.).

## Capabilities

### New Capabilities
- `audio-cascade-detection`: Implements the multi-stage (RMS -> DSP -> TinyML) detection logic.
- `esp32-s3-power-mgmt`: Logic for Deep Sleep, RTC memory persistence, and wake-on-interrupt from I2S/MEMS.
- `tinyml-classifier`: Integration of sound classification models into the firmware.

### Modified Capabilities
- None.

## Impact

- **Firmware**: Major new implementation for the ESP32-S3 target.
- **Hardware**: Specifically targets the ESP32-S3 and its specific power-saving features.
- **Model**: Requires a trained TinyML model (e.g., exported from Edge Impulse) to be bundled with the firmware.
