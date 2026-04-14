## Why

Currently, hardware testing of the acoustic monitoring nodes and their LoRa mesh communication relies either on physical deployments or simplistic Dockerized environments that do not fully capture hardware interactions. By integrating Renode, we can simulate the exact target architecture (Raspberry Pi 3B / ARM microcontrollers) alongside a simulated LoRa radio medium. This provides a repeatable, faithful environment for testing chainsaw and fauna detection and the subsequent mesh packet transmission without requiring real hardware.

## What Changes

- Add a fully specified Renode simulation environment for the Patronos Forest Guardian project.
- Implement simulation configurations to deploy at least 5 nodes interacting via simulated LoRa networks.
- Specify how simulated audio data (e.g. `.wav` files containing chainsaw or animal sounds) is ingested into the simulated nodes' I2S microphone drivers to trigger AI models.

## Capabilities

### New Capabilities
- `renode-mesh-simulation`: Configuration and scripting to run a multi-node LoRa mesh simulation in Renode, including radio medium simulation.
- `renode-audio-ingestion`: Mechanism to inject external audio samples (chainsaw and fauna) into the simulated I2S audio interface of specific nodes during Renode execution.

### Modified Capabilities
- `lorawan-mesh-routing`: We may need to adapt or verify the routing mechanisms to ensure they operate correctly within the simulated radio constraints of Renode.

## Impact

- Testing infrastructure will be significantly enhanced, allowing for CI/CD integration of hardware-in-the-loop-like tests.
- Requires Renode v1.16+ installed.
- No direct impact on production firmware code, but may require specific simulated peripherals if hardware ones are too complex.
