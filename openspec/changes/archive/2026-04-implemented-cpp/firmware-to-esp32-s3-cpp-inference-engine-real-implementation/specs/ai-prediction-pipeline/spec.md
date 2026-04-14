## ADDED Requirements

### Requirement: End-to-End Prediction Flow
The system SHALL maintain a consistent data flow from audio capture to classification.

#### Scenario: Continuous Detection
- **WHEN** the system is running in active mode
- **THEN** audio blocks MUST be processed by the DSP pipeline and then classified by the `InferenceEngine` without losing samples

### Requirement: Classification Logging
The system SHALL log the classification results to the serial console for debugging and telemetry.

#### Scenario: Telemetry Update
- **WHEN** a classification is complete
- **THEN** the system SHALL output a formatted string containing the label and confidence (e.g., "[AI] Result: Chainsaw (0.95)")
