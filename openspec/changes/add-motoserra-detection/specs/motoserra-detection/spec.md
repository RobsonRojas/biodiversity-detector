# OpenSpec Proposal - Robson Rojas
# Project: Guardian-Edge-AI-V1
spec_version: "1.0.0"

metadata:
  project_name: "Acoustic Monitoring"
  author: "Robson Rojas Andrade"
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

### Requirement: core-detection
The system MUST identify chainsaw (motoserra) sounds from raw audio data using a quantized TFLite Micro model.

#### Scenario: positive-detection
- **WHEN** audio buffer contains a 1-second segment matching the chainsaw spectral profile with >0.85 confidence
- **THEN** an alert is triggered immediately

#### Scenario: false-positive-suppression
- **WHEN** audio buffer contains bird songs or wind noise
- **THEN** no alert is triggered and confidence score remains < 0.2