# Guardian Firmware Development Guide

This guide provides code examples and integration patterns for the ESP32-S3 C++ firmware.

## 1. I2S Configuration (Standard Driver)

The firmware uses the ESP-IDF v5.x standardized I2S driver for high-performance audio capture.

```cpp
#include "driver/i2s_std.h"

void init_i2s() {
    i2s_chan_handle_t rx_handle;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_new_channel(&chan_cfg, NULL, &rx_handle);

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000), // 16kHz
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = GPIO_NUM_14,
            .ws = GPIO_NUM_15,
            .dout = I2S_GPIO_UNUSED,
            .din = GPIO_NUM_32,
        },
    };
    i2s_channel_init_std_mode(rx_handle, &std_cfg);
    i2s_channel_enable(rx_handle);
}
```

## 2. DSP Verification (ESP-DSP)

Before running the AI model, we perform a spectral analysis to confirm if the audio signal resembles a motor.

```cpp
#include "dsps_fft242.h"

bool verify_spectral_profile(float* data, int points) {
    // 1. Apply Hann Window
    dsps_wind_hann_f32(data, points);
    
    // 2. Perform Real FFT
    dsps_fft2r_f32(data, points);
    dsps_bit_rev_f32(data, points);
    dsps_cplx2re_f32(data, points);

    // 3. Analyze energy in the chainsaw harmonic range (200Hz - 800Hz)
    float target_energy = 0;
    for (int i = bin_200hz; i < bin_800hz; i++) {
        target_energy += data[i] * data[i];
    }
    
    return target_energy > SPECTRUM_THRESHOLD;
}
```

## 3. AI Inference Integration

```cpp
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

void run_inference(signal_t* signal) {
    ei_impulse_result_t result = { 0 };
    
    // Run the TFLite Micro classifier
    EI_IMPULSE_ERROR res = run_classifier(signal, &result, false);
    
    if (res == EI_IMPULSE_OK) {
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            if (result.classification[ix].value > 0.8) {
                printf("Detected: %s Score: %.2f\n", 
                       result.classification[ix].label, 
                       result.classification[ix].value);
            }
        }
    }
}
```
## 4. Building instructions

To build the firmware, ensure you have the ESP-IDF environment exported:

```bash
cd src/esp32-s3-cpp
idf.py set-target esp32s3
idf.py build
```
