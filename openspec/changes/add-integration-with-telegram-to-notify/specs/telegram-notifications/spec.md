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
