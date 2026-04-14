## ADDED Requirements

### Requirement: TFLite Micro Inference
The system SHALL run the chainsaw detection model using TensorFlow Lite for Microcontrollers.

#### Scenario: Successful inference
- **WHEN** a 1-second audio frame (16000 samples) is provided
- **THEN** the model returns a confidence score between 0.0 and 1.0

### Requirement: Static Memory Allocation
The system SHALL use a statically allocated tensor arena for TFLite Micro to prevent heap fragmentation.

#### Scenario: Model initialization
- **WHEN** the detection engine is initialized
- **THEN** it successfully allocates memory within the predefined static tensor arena boundaries
