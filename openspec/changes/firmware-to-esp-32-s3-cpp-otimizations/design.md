## Context

The current firmware implementation relies on the standard ESP-IDF I2S driver. While robust, this driver involves multiple layers of abstraction and data copying (from DMA buffers to user-space buffers), which increases latency and CPU overhead. For high-fidelity biodiversity monitoring and real-time chainsaw detection, we need a more direct path from the microphone to the AI inference engine.

## Goals / Non-Goals

**Goals:**
- Implement a direct register-level interface for the ESP32-S3 I2S controller.
- Design a Shared Memory Ring Buffer to allow the I2S DMA to write directly into memory accessible by the DSP pipeline without intermediate copies.
- Minimize audio capture latency to sub-millisecond levels (excluding buffer accumulation time).
- Improve memory safety by using a fixed-allocation ring buffer with strict boundary checks.

**Non-Goals:**
- Supporting multiple I2S microphones simultaneously (focus on primary S3 I2S0).
- Replacing the entire ESP-IDF driver architecture; instead, we will use a hybrid approach or a specialized low-level driver for the audio path.

## Decisions

- **Direct SoC Register Interface**: We will bypass the high-level `driver/i2s_std.h` and use the ESP-IDF `hal/i2s_ll.h` (Low Level) or direct register mappings to configure the I2S peripheral and DMA descriptors.
- **Zero-Copy Architecture**: The DMA descriptors will be configured to point directly to segments of our Shared Memory Ring Buffer. This allows the hardware to fill the buffer while the application reads from the previous segments simultaneously.
- **High-Priority Interrupts**: The I2S EOF (End of Frame) interrupt will be handled in a high-priority ISR to quickly update the ring buffer pointers and signal the DSP task.

## Risks / Trade-offs

- **Technical Complexity**: Register-level coding is more error-prone and harder to debug than using standard APIs.
- **Portability**: This design is specific to the ESP32-S3 hardware architecture and will require significant refactoring for other ESP32 variants.
- **Memory Pressure**: Using large, fixed DMA-capable memory blocks reduces the available heap for other system functions.
