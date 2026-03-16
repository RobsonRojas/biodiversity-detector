## Why

Detecting chainsaws in the forest is only half the battle; real-time alerting is critical for stopping illegal logging. This change integrates a Telegram notification system that alerts agents the moment a detection is confirmed, providing an immediate and accessible communication channel for the forest guards.

## What Changes

1.  **Notification Gateway**: Integration with a Telegram Bot API to send alerts.
2.  **Alert Formatting**: Implementation of a message formatter to include confidence scores, timestamps, and device identifiers.
3.  **Connectivity Handling**: Resilient message queuing to handle intermittent connectivity (Mesh-Networking/LoRaWAN gateway sync).
4.  **C++ 26 Integration**: Use of modern C++ features (e.g., `std::expected` for error handling in the API handshake) matching the engine's software stack.

## Capabilities

### New Capabilities
- `telegram-notifications`: Automated alerting system via Telegram Bot API triggered by the detection engine.

### Modified Capabilities
- (None)

## Impact

- **Telemetry System**: Extends the existing telemetry protocol to include Telegram-specific payloads.
- **Security**: Requires secure storage of the Telegram Bot Token (e.g., via environment variables or encrypted configuration).
- **Communication Layer**: Intersects with the LoRaWAN/Mesh-Networking layer to ensure messages reach the internet-connected gateway.
Lines of Code: ~200-300 new lines for the notification handler.
Dependencies: `libcurl` or a lightweight C++ HTTP client for API interaction.
