## 1. Environment & Scaffold

- [ ] 1.1 Configure the Rust-for-Linux toolchain and kernel headers.
- [ ] 1.2 Create the module scaffold with `module_guardian_audio.rs`.
- [ ] 1.3 Implement the `Kbuild` and `Makefile` for kernel module compilation.

## 2. Character Device Implementation

- [ ] 2.1 Register the character device major/minor numbers.
- [ ] 2.2 Implement the `file_operations` for `open`, `release`, and `read`.
- [ ] 2.3 Create the `/dev/forest_audio` node with correct permissions.

## 3. Real Hardware I2S & DMA Logic

- [x] 3.1 Implement BCM2837 Register Mappings for PCM and DMA.
- [x] 3.2 Configure DMA Channel 2 for PCM-to-Ring-Buffer transfers.
- [x] 3.3 Implement the `mmap` kernel logic for zero-copy shared memory.
- [x] 3.4 Implement hardware interrupt handler for buffer boundaries.

## 4. Verification & Hardening

- [x] 4.1 Verify module loading/unloading without kernel panics.
- [ ] 4.2 Use a scope/logic analyzer to verify I2S clock and data signals.
- [ ] 4.3 Validate buffer synchronization with a test user-space application.
- [x] 4.4 Implement and verify host simulation mode using custom device paths.
