## 1. Renode Platform Setup

- [ ] 1.1 Create `sim/esp32s3.repl` file defining the Xtensa core, memory layout, and UART/I2S peripherals.
- [ ] 1.2 Develop `sim/main.resc` to load the ESP32-S3 ELF binary and start the emulation.
- [ ] 1.3 Verify basic UART logging from the firmware within Renode.

## 2. Audio & Peripheral Emulation

- [ ] 2.1 Implement a Renode extension or host-side bridge to feed `.wav` samples into the emulated I2S bus.
- [ ] 2.2 Configure RTC "wake-up" hooks in Renode to simulate power-saving transitions.
- [ ] 2.3 Create a "WOS" (Wake-on-Sound) trigger script for the simulation.

## 3. Network & Simulator Integration

- [ ] 3.1 Update `SimulationRunner` (C++/Python) to support spawning Renode instances as node backends.
- [ ] 3.2 Implement a socket bridge between Renode's emulated UART/SPI and the host's `MeshRelay`.
- [ ] 3.3 Add `BACKEND=renode` support to the `.env` configuration file.

## 4. Validation & Benchmarking

- [ ] 4.1 Run a mixed simulation with 1 Gateway (Native) and 2 Renode Nodes.
- [ ] 4.2 Verify alert propagation from a Renode node through the mesh to Supabase.
- [ ] 4.3 Benchmark simulation real-time factor (RTF) when running multiple Renode instances.
