## ADDED Requirements

### Requirement: multi-class-inference
The system SHALL use a TFLite Micro model to classify incoming audio into multiple categories: chainsaw, birds, rain, and other forest sounds.

#### Scenario: detect-birds
- **WHEN** audio containing bird songs is processed
- **THEN** the system MUST identify the 'birds' class with confidence > 0.7

#### Scenario: detect-chainsaw-and-birds
- **WHEN** both chainsaw and bird sounds are present
- **THEN** the system MUST prioritize the 'chainsaw' alert due to its high-risk nature
