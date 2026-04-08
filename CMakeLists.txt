cmake_minimum_required(VERSION 3.25)
project(GuardianEdgeAI CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# AI Engine: TFLite Micro
# In a real environment, this would be a FetchContent or a submodule
# For this implementation, we assume tflite-micro headers are in include/tflite-micro
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Optimization flags for Raspberry Pi 3 Model B (BCM2837 aarch64)
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    add_compile_options(-O3 -march=armv8-a+simd -mcpu=cortex-a53)
endif()

# External Dependencies
find_package(CURL REQUIRED)

# Core Guarding Logic
add_library(GuardianCore
    src/audio/AudioIn.cpp
    src/audio/CascadeFilter.cpp
    src/ai/DetectionEngine.cpp
    src/ai/EdgeImpulseWrapper.cpp
    src/AlertManager.cpp
    src/telemetry/LoRaDriver.cpp
    src/telemetry/SimulatedLoRaDriver.cpp
    src/telemetry/PhysicalLoRaDriver.cpp
    src/telemetry/MeshProtocol.cpp
    src/telemetry/ConfigParser.cpp
    src/telemetry/TelegramClient.cpp
    src/telemetry/RTCStore.cpp
    src/hal/esp32/ESP32Power.cpp
    src/hal/esp32/I2SCapture.cpp
)
target_link_libraries(GuardianCore PUBLIC CURL::libcurl)

# Main Application
add_executable(guardian_node src/main.cpp)
target_link_libraries(guardian_node GuardianCore)

# Task tracking for OpenSpec (Simulated)
add_custom_target(openspec_sync
    COMMAND openspec task complete "1.1" --change "add-motoserra-detection"
)
