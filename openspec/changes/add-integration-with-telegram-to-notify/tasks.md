## 1. Credentials & Environment

- [x] 1.1 Securely store `TELEGRAM_BOT_TOKEN` and `TELEGRAM_CHAT_ID`.
- [x] 1.2 Verify network reachability to `api.telegram.org` from the gateway environment.

## 2. Notification Client Implementation

- [x] 2.1 Implement the `TelegramClient` class using C++ 26.
- [x] 2.2 Integrate `libcurl` for asynchronous HTTPS requests.
- [x] 2.3 Implement robust error handling using `std::expected` for API handshakes.

## 3. Message Formatting & Queuing

- [x] 3.1 Create an alert formatter using `std::print` for HTML-formatted payloads.
- [x] 3.2 Implement a persistent local queue (SQLite or flat file) for offline tolerance.

## 4. System Integration

- [x] 4.1 Subscribe to detection events from the AI engine.
- [x] 4.2 Add unit tests for message formatting and retry logic.
- [x] 4.3 Perform end-to-end verification with a test bot.
