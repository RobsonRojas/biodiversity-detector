## Why

The current simulation environment uses native Linux processes (`GuardianNode`) to simulate leaf nodes. While effective for logic validation, it does not account for hardware-specific constraints of the target ESP32-S3 platform, such as RTC memory behavior, peripheral power management, and real-time audio DMA constraints. 

Integrating Renode allows us to run the actual ESP32-S3 C++ firmware in an emulated environment, providing a higher level of confidence before physical deployment.

## What Changes

- Introduction of a Renode simulation environment for the ESP32-S3.
- Creation of Renode script (`.resc`) and platform files (`.repl`) to model the ESP32-S3 and its MEMS/I2S peripherals.
- Integration of the Renode simulation into the existing mesh network simulator to allow mixed-mode simulation (native processes + emulated hardware).

## Capabilities

### New Capabilities
- `renode-simulation`: Hardware-accurate emulation of the ESP32-S3 firmware using Renode.
- `peripherals-emulation`: Mocking of I2S audio input and RTC power states within Renode.

### Modified Capabilities
- `mesh-network-simulator`: Updated to support spawning and communicating with Renode instances as node backends.

## Impact

- **Affected Systems**: `SimulationRunner`, `MeshNode` abstraction.
- **Dependencies**: Requires `Renode` to be installed in the simulation environment.
- **APIs**: New management API/scripts to start/stop Renode instances and inject audio samples into the emulated I2S bus.
