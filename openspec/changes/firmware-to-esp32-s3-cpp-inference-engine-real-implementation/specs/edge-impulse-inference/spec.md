## ADDED Requirements

### Requirement: Real-Time Model Inference
The inference engine SHALL execute the Edge Impulse model classification on the provided audio signal features.

#### Scenario: Successful Classification
- **WHEN** valid normalized float features are provided to the `classify` method
- **THEN** the engine SHALL return the most probable label and its confidence score

### Requirement: PSRAM Static Allocation
The TFLite tensor arena SHALL be allocated in PSRAM to preserve internal SRAM.

#### Scenario: Initialization Success
- **WHEN** the `init` method is called
- **THEN** the engine SHALL successfully allocate memory in PSRAM and load the model parameters
