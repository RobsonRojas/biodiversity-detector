## ADDED Requirements

### Requirement: Simulate Multi-Node Topology
The simulation environment SHALL define at least 5 distinct machine models representing the sensor nodes and gateways.

#### Scenario: Network instantiation
- **WHEN** the Renode simulation script is started
- **THEN** it instantiates 5 interconnected virtual machines sharing a common LoRa radio medium

### Requirement: Execute Native Firmware
The Renode machines SHALL load and execute the `motoserra-detect-system` binary natively compiled for the target architecture.

#### Scenario: Firmware execution
- **WHEN** the simulation starts
- **THEN** each machine boots and runs the `guardian_node` logic
