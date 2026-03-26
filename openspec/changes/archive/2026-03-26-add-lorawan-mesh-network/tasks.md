## 1. LoRaWAN Physical Layer integration

- [x] 1.1 Integrate with the LoRaWAN driver (AU915) to send/receive raw packets.
- [x] 1.2 Implement a buffer manager using `std::span` for zero-copy packet handling.

## 2. Static Mesh Routing Logic

- [x] 2.1 Define the Mesh packet header and serialization logic.
- [x] 2.2 Implement the Static Routing table using `std::flat_map`.
- [x] 2.3 Implement the `RouteManager` to handle multi-hop forwarding based on static config.

## 3. Configuration & Integrity

- [x] 3.1 Implement a static configuration parser for node roles and routes.
- [x] 3.2 Implement packet signature verification for secure message delivery.

## 4. Integration

- [x] 4.1 Update `AlertManager` to forward local detections as Mesh packets.
- [x] 4.2 Implement the Gateway logic to receive Mesh alerts and push to the Telegram client.
- [x] 4.3 Verify network delivery using pre-defined static paths.
