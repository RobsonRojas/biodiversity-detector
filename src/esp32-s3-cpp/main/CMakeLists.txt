set(srcs 
    "main.cpp"
    "audio_capture.cpp"
    "audio_buffer.cpp"
    "dsp_analyzer.cpp"
    "inference_engine.cpp"
    "esp_lora_driver.cpp"
    "battery_monitor.cpp"
    "telemetry_manager.cpp"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp-dsp esp_adc)
