# OpenSpec Proposal - Robson Rojas
# Project: Guardian-Edge-AI-V1
spec_version: "1.0.0"

metadata:
  project_name: "Acoustic Monitoring"
  author: "Robson Rojas"
  target_hardware: "Raspberry-Pi-aarch64"
  deployment_env: "Amazon Rainforest (High Humidity / Solar Powered)"

# Especificação da Interface do Driver (Rust Kernel Module)
interface:
  transport: "Shared-Memory-Ring-Buffer"
  device_node: "/dev/forest_audio"
  data_format:
    sample_rate: 16000     # Hz
    bit_depth: 32          # INT32 (Alinhado para SIMD no C++ 26)
    channels: 1            # Mono
    frame_size: 16384      # 1 segundo de buffer @ 16kHz

# Requisitos de Software (C++ 26 Engine)
software_stack:
  language_standard: "c++26"
  features_required:
    - "std::mdspan"        # Para manipulação matricial do espectrograma
    - "std::expected"      # Tratamento de erro robusto no kernel-handshake
    - "std::simd"          # Aceleração da FFT (se suportado pelo compilador)
  ai_framework: "TFLite-Micro"

# Orçamento Energético e Restrições de Borda
constraints:
  max_power_draw_mw: 400
  low_power_strategy: "Duty-Cycling"
  wake_on_sound: true      # O driver Rust deve acordar o processo C++
  network:
    primary: "LoRaWAN (AU915)"
    secondary: "Mesh-Networking (Reticulum)"

## ADDED Requirements

### Requirement: i2s-ingestion
The system MUST ingest audio data from the `/dev/forest_audio` device using the I2S protocol.

#### Scenario: continuous-streaming
- **WHEN** the system is in active state (5 seconds duty cycle)
- **THEN** it reads 16,000 samples per second into the ring buffer
