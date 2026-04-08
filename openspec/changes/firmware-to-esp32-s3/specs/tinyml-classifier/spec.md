## ADDED Requirements

### Requirement: Real-time Audio Pre-processing
The firmware MUST include a feature extraction pipeline that converts raw PCM audio into the input format required by the TinyML model (e.g., Log-Mel Spectrogram).

#### Scenario: Spectral Analysis
- **WHEN** the DSP stage triggers the AI stage.
- **THEN** the system must calculate a 256-point FFT with Hanning windowing every 16ms for a 1-second audio window.

### Requirement: Multi-Class Detection Accuracy
The model MUST classify sounds into categories including "Chainsaw", "Animal", "Insect", and "Silence/Background".

#### Scenario: Chainsaw Detection
- **WHEN** a chainsaw sound is processed.
- **THEN** the model's confidence score must exceed 0.85 before an alert is triggered.

#### Scenario: Ambiguity Protection
- **WHEN** the classification confidence is below 0.5 for all classes.
- **THEN** the event must be logged as "Unknown" and no telemetry should be sent to conserve LoRaWAN bandwidth.
