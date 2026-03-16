## Context

The system currently detects chainsaws on the edge but needs a way to push these alerts to human responders. This design covers the software component that bridges the detection events to the Telegram Bot API.

## Goals / Non-Goals

**Goals:**
- Implement a robust Telegram notification client in C++ 26.
- Use `std::expected` for error propagation during API calls.
- Support HTML-formatted messages for better readability.
- Implement a local queue for offline tolerance.

**Non-Goals:**
- Implementation of the Android/iOS Telegram app itself.
- High-bandwidth data transfer (e.g., sending raw audio clips).
- Managing Telegram group permissions.

## Decisions

- **C++ 26**: Aligning with the engine's stack to allow shared utilities and maintainability.
- **libcurl**: Chosen for its industry-standard reliability in embedded Linux environments.
- **Token Management**: Using environment variables to avoid hardcoding credentials in the OpenSpec change.
- **Std::Expected**: Replacing traditional try/catch or error codes with modern C++ error handling for the network layer.

## Risks / Trade-offs

- **Internet Dependency**: Telegram requires a working internet connection. In the deep forest, this relies on the LoRaWAN gateway providing an internet bridge.
- **Latency**: HTTPS requests have higher latency than local processing, but this is acceptable for notification purposes (target < 5s from gateway receipt).
