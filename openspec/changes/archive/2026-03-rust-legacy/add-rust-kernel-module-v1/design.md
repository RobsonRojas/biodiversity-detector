## Context

The system currently relies on user-space audio ingestion, which is prone to latency and data loss under high CPU load. We need a robust, memory-safe kernel-level driver to handle raw I2S data from the microphone and provide it to the C++ detection engine via a shared-memory buffer.

## Goals / Non-Goals

**Goals:**
- Implement a character device (`/dev/forest_audio`).
- Achieve zero-copy data transfer between kernel and user space.
- Utilize Rust for Linux to ensure memory safety in the kernel.
- Support 16kHz MONO audio @ 32-bit depth.

**Non-Goals:**
- Implementation of audio effects or filtering (handled in user space).
- Integration with ALSA or PulseAudio (bypass for minimal latency).
- Support for multiple concurrent readers (exclusive access for `guardian_node`).

## Decisions

- **Buffer Architecture**: A lock-less Shared-Memory Ring Buffer. The kernel writes samples, and the user-space process reads them using `mmap`.
- **Synchronization**: Use a simple sequence counter to detect buffer overflows/under-runs, minimizing interrupt latency.
- **Language**: Use the `kernel` crate provided by the Rust-for-Linux project to interface with GPL-exported kernel symbols.

## Risks / Trade-offs

- **Toolchain Complexity**: Dependency on `rustc` and specific kernel configurations (`CONFIG_RUST=y`).
- **Hardware Lock-in**: Direct register access requires specific mappings for the target RPi 3 BCM2837 SoC.
- **Performance**: While zero-copy is efficient, the overhead of context switching for interrupts must be carefully managed to avoid power spikes.
