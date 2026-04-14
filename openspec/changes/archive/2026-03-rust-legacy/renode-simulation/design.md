## Context
The Patronos Forest Guardian project currently relies on Docker-based UDP simulations to test its mesh network capabilities. While this validates the protocol logic, it abstracts away all hardware interactions, such as CPU scheduling, actual peripheral access (I2S audio, SPI for LoRa), and memory constraints. By utilizing Renode, an open-source simulation framework for multi-node systems, we can model the exact target hardware (e.g., Raspberry Pi 3B / ARM Cortex-A) executing the real firmware binaries, leading to much higher confidence before physical deployment.

## Goals / Non-Goals

**Goals:**
- Define a complete multi-node simulated environment in Renode (`.resc` scripts).
- Connect the simulated LoRa interfaces of at least 5 nodes through Renode's `WirelessMedium` for realistic packet exchange.
- Provide a mechanism to ingest pre-recorded `.wav` audio files (fauna and chainsaw sounds) into the simulated I2S peripheral of a node.

**Non-Goals:**
- Highly realistic RF interference or path loss modeling (Renode's abstraction is sufficient for functional testing).
- Deprecating the current fast, software-only Docker simulation; Renode will complement it for deeper hardware-in-the-loop-style testing.

## Decisions

- **Renode `.resc` Scripting**: We will write one or more `.resc` scripts to instantiate the 5 machine models. They will share a global `WirelessMedium`.
- **Target Platform**: We will simulate the `raspberry-pi-3b` (or equivalent supported ARM architecture) as specified in `openspec/config.yaml`.
- **LoRa Simulation**: We'll use Renode's existing Semtech LoRa models (e.g., SX1276) attached via SPI to the main CPU, allowing the actual `LoRaDriver` code to execute with real SPI transactions.
- **Audio Ingestion**: To simulate acoustic monitoring, we will utilize Renode's capabilities to map external signals or files to a simulated I2S or memory-mapped audio peripheral. We might write a small C# or Python peripheral proxy if the native I2S model lacks direct file streaming.

## Risks / Trade-offs
- **[Risk] Simulation Overhead**: Running 5 full OS or complex RTOS environments may be computationally heavy. 
  → *Mitigation*: Leverage Renode's virtual time to guarantee correct synchronization and behavior, even if the simulation runs slower than real time.
- **[Risk] Peripheral Availability**: Renode might not have the exact I2S / DMA peripheral model for the chosen SoC out of the box. 
  → *Mitigation*: We will implement a mock peripheral in Python/C# for Renode that just feeds the `.wav` buffer into memory and triggers the appropriate interrupts.
