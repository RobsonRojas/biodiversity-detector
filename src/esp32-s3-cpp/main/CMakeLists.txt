set(srcs 
    "main.cpp"
    "audio_capture.cpp"
    "audio_buffer.cpp"
    "dsp_analyzer.cpp"
    "inference_engine.cpp"
    "esp_lora_driver.cpp"
    "battery_monitor.cpp"
    "telemetry_manager.cpp"
    "i2s_low_level.cpp"
    "shared_ring_buffer.cpp"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_spi esp-dsp esp_adc esp_driver_gpio esp_timer hal esp_hw_support esp_rom esp_driver_i2s)
