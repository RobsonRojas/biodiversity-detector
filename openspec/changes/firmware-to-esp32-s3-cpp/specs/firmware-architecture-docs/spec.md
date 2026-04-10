## ADDED Requirements

### Requirement: architectural-documentation
The firmware MUST include a comprehensive architectural guide explaining its components and flows.

#### Scenario: system-overview
- **WHEN** a developer reads the architecture documentation
- **THEN** they must find a clear explanation of the 3-stage power cascade (Quiet, Transition, Active).

#### Scenario: component-integration-guide
- **WHEN** a developer needs to integrate a new peripheral or model
- **THEN** they must find documented integration points and lifecycle hooks.

### Requirement: code-examples
The documentation MUST include working code examples for critical subsystems.

#### Scenario: peripheral-usage-examples
- **WHEN** viewing the documentation
- **THEN** examples for I2S setup, LoRa packet transmission, and TFLite Micro invocation must be present.
