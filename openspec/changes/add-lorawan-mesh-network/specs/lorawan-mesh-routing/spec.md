# OpenSpec Proposal - Robson Rojas
# Project: Guardian-Edge-AI-V1
spec_version: "1.0.0"

metadata:
  project_name: "LoRaWAN Mesh Routing"
  author: "Antigravity (AI Assistant)"
  target_hardware: "Raspberry-Pi-aarch64"
  deployment_env: "Amazon Rainforest"

# Especificação da Camada de Rede (Static Mesh Protocol)
interface:
  physical_layer: "LoRaWAN (AU915)"
  topology: "Static Directed Mesh"
  max_hops: 7
  packet_format:
    header_size: 16        # Bytes
    signature: "ED25519"   # Assinatura digital para pacotes de dados
    payload_max: 200       # Bytes (limitado pelo tempo de ar do LoRa)

# Requisitos de Software (C++ 26)
software_stack:
  language_standard: "c++26"
  features_required:
    - "std::span"           # Para manipulação de buffers de pacotes sem cópia
    - "std::expected"       # Tratamento de erro robusto no roteamento
    - "std::flat_map"       # Tabela de roteamento eficiente (C++ 23/26)

# Algoritmo de Roteamento
routing:
  strategy: "Static Path Selection"
  config_method: "Compile-time or Protected Flash"
  table_size_max: 32       # Entradas de roteamento estático
