# OpenSpec Proposal - Robson Rojas
# Project: Guardian-Edge-AI-V1
spec_version: "1.0.0"

metadata:
  project_name: "Telegram Notification Integration"
  author: "Antigravity (AI Assistant)"
  target_hardware: "Raspberry-Pi / Cloud Gateway"
  deployment_env: "Hybrid (Edge to Cloud)"

# Especificação da Interface de Notificação
interface:
  protocol: "HTTPS / Telegram Bot API"
  endpoint: "https://api.telegram.org/bot<token>/sendMessage"
  data_format:
    encoding: "UTF-8"
    content_type: "application/json"
    payload_schema:
      chat_id: "int64"
      text: "string"
      parse_mode: "HTML"

# Requisitos de Software (C++ 26)
software_stack:
  language_standard: "c++26"
  features_required:
    - "std::expected"      # Para tratamento de erros na comunicação HTTP
    - "std::print"         # Para formatação de logs e mensagens (C++ 23/26)
  libraries:
    - "libcurl"            # Cliente HTTP estável para ambientes Linux
    - "nlohmann/json"      # Manipulação de JSON moderna

# Restrições e Segurança
constraints:
  security:
    bot_token: "Environment Variable (TELEGRAM_BOT_TOKEN)"
    chat_id: "Environment Variable (TELEGRAM_CHAT_ID)"
  reliability:
    retry_strategy: "Exponential Backoff"
    offline_queuing: true   # Armazenar alertas se a conexão cair

## ADDED Requirements

### Requirement: telegram-alert-delivery
The system MUST be able to send acoustic detection alerts to a configured Telegram chat using the Bot API.

#### Scenario: successful-delivery
- **WHEN** the system sends a valid HTTPS POST request to the Telegram API
- **THEN** it receives a 200 OK response and the message is delivered

#### Scenario: connectivity-fallback
- **WHEN** the Telegram API is unreachable due to network issues
- **THEN** the system MUST queue the alert locally and retry with exponential backoff
