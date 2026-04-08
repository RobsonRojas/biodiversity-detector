## 1. Environment Preparation
- [x] 1.1 Update `Dockerfile.sim` to include the `qemu-system-xtensa` binary and necessary shared libraries.
- [x] 1.2 Verify that QEMU can be executed inside the simulation container without requiring a display (headless mode).

## 2. Firmware & Image Management
- [x] 2.1 Develop a helper script to verify the presence of `bootloader.bin`, `partition-table.bin`, and `esp32.bin` in the `dist/` directory.
- [x] 2.2 Configure QEMU arguments to load these binary components at their respective flash offsets (e.g., 0x0, 0x8000, 0x10000).

## 3. QEMU Simulation Runner
- [x] 3.1 Create `scripts/qemu_runner.py` as a replacement/alternative to `renode_runner.py`.
- [x] 3.2 Implement the UART bridge in `qemu_runner.py` using QEMU's `-serial telnet:localhost:PORT,server,nowait` or similar socket interface.
- [x] 3.3 Ensure logs from the QEMU serial output are captured and mirrored to the simulation runner's stdout.

## 4. System Integration & Mesh Simulation
- [x] 4.1 Update `docker-compose.sim.yml` to define a heterogeneous 5-node topology (e.g., node1 as a QEMU ESP32-S3 node, nodes 2-5 as Native Linux nodes).
- [x] 4.2 Verify static routing propagation through the QEMU node to the gateway and then to the Web Manager.

## 5. Validation
- [/] 5.1 Execute the full mesh simulation and confirm that a simulated alert from a leaf node reaches the Supabase backend.
- [/] 5.2 Validate that the UART-to-UDP bridge is stable and does not leak sockets or memory during long simulation runs.
