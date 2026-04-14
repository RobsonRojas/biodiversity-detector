## Context

The current simulation environment uses native binaries for both the Gateway and the Sensor Nodes. While this validates high-level logic, it misses hardware-specific bugs related to memory constraints and peripheral timing on the ESP32-S3. Renode provides a way to run the exact same binary that would be flashed to the hardware, allowing for multi-node simulation with high-fidelity hardware modeling.

## Goals / Non-Goals

**Goals:**
- Provide a Renode configuration (`.resc`) that boots the ESP32-S3 firmware.
- Implement an automated bridge that forwards "virtual audio" from the simulator files into the emulated I2S bus.
- Support multi-instance Renode simulation (modeling multiple nodes concurrently).

**Non-Goals:**
- Emulating the exact power consumption in nano-amps (simulated states only).
- Simulating the full LoRa radio wave propagation (Renode will use a socket-based bridge for mesh networking).

## Decisions

- **Xtensa Core Modeling**: Use Renode's Xtensa LX7 model to run the ESP32-S3 firmware binaries.
- **Peripheral Bridging**: Use Renode's `FileSource` or a custom extension to feed raw audio `.wav` files into the emulated microphone's I2S data line.
- **Network Simulation**: Connect Renode's emulated UART/SPI to a host-side Python or C++ "Mesh Relay" that connects them into the existing `LoRaSim` network.

## Risks / Trade-offs

- **Performance**: Running multiple Renode instances is significantly heavier than native processes. This may limit the scale of simulations to 5-10 nodes compared to 50+ natively.
- **Fidelity**: Some ESP32-S3 specific registers (especially regarding the AI Accelerator/ESP-NN) might not be fully modeled in open-source Renode, requiring custom peripheral mocks.
