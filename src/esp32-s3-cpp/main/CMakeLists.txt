set(srcs 
    "main.cpp"
    "audio_capture.cpp"
    "dsp_analyzer.cpp"
    "inference_engine.cpp"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp-dsp esp_nn)
