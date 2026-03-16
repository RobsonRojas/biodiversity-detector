# OpenSpec Proposal - Robson Rojas
# Project: Guardian-Edge-AI-V1
spec_version: "1.0.0"

metadata:
  project_name: "Static Topology Management"
  author: "Antigravity (AI Assistant)"

# Gestão de Topologia Estática
topology:
  discovery_mode: "Disabled"
  config_source: "Static Configuration File"
  validation: "Checksum-on-boot"

# Gerenciamento de Papéis de Nó
node_roles:
  - id: "leaf"
    can_relay: false
    duty_cycle: "low"
  - id: "router"
    can_relay: true
    duty_cycle: "medium"
  - id: "gateway"
    can_relay: true
    internet_bridge: true
    duty_cycle: "high"

# Sincronização de Tempo (Pre-configured)
sync:
  accuracy_ms: 100
  drift_tolerance_ppm: 50
