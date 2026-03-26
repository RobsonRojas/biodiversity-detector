#pragma once

#include "telemetry/TelegramClient.hpp"
#include "telemetry/AlertFormatter.hpp"
#include "telemetry/OfflineQueue.hpp"
#include "telemetry/LoRaDriver.hpp"
#include "telemetry/ConfigParser.hpp"
#include "telemetry/MeshProtocol.hpp"
#include <memory>
#include <iostream>

namespace guardian {

class AlertManager {
public:
    AlertManager(const telemetry::NodeConfig& config,
                 std::shared_ptr<telemetry::TelegramClient> client,
                 std::shared_ptr<telemetry::OfflineQueue> queue,
                 std::shared_ptr<telemetry::LoRaDriver> lora)
        : config_(config), client_(std::move(client)), queue_(std::move(queue)), lora_(std::move(lora)) {}

    void on_detection(const telemetry::DetectionEvent& event) {
        if (event.confidence < 0.85f) {
            return;
        }

        std::string message = telemetry::AlertFormatter::format_detection(event);
        
        if (config_.role == telemetry::NodeRole::Gateway) {
            send_to_telegram(message);
        } else {
            send_via_mesh(message);
        }
    }
    
    void on_mesh_receive(std::span<const uint8_t> data) {
        auto pkt = telemetry::MeshPacket::deserialize(data);
        if (pkt && pkt->verify_signature()) {
            if (config_.role == telemetry::NodeRole::Gateway && pkt->header.target_id == config_.node_id) {
                std::string msg(pkt->payload.begin(), pkt->payload.end());
                std::cout << "[Gateway] Received Mesh Alert. Forwarding to Telegram.\n";
                send_to_telegram(msg);
            } else if (config_.role == telemetry::NodeRole::Router || config_.role == telemetry::NodeRole::Gateway) {
                std::cout << "[Router] Forwarding packet...\n";
                if (pkt->header.hop_limit > 0) {
                    pkt->header.hop_limit--;
                    auto next_hop = config_.route_manager.get_next_hop(pkt->header.target_id);
                    if (next_hop && lora_) {
                        lora_->send(pkt->serialize());
                    }
                }
            }
        }
    }

private:
    void send_to_telegram(const std::string& message) {
        if (!client_) return;
        auto result = client_->send_message(message);
        if (!result) {
            std::cerr << "Falha ao enviar mensagem para o Telegram. Armazenando em fila offline." << std::endl;
            if (queue_) queue_->push(message);
        } else {
            if (queue_) {
                auto pending = queue_->pop_all();
                for (const auto& msg : pending) {
                    client_->send_message(msg);
                }
            }
        }
    }

    void send_via_mesh(const std::string& message) {
        if (!lora_) return;
        telemetry::MeshPacket pkt;
        pkt.header.target_id = 0x0001; // Gateway ID
        pkt.header.sender_id = config_.node_id;
        pkt.header.hop_limit = 7;
        pkt.header.payload_len = message.size();
        pkt.header.signature = 0xABCD;
        pkt.payload.assign(message.begin(), message.end());
        
        auto next_hop = config_.route_manager.get_next_hop(pkt.header.target_id);
        if (next_hop) {
            std::cout << "[Leaf/Router] Sending detection alert via Mesh to hop " << *next_hop << "\n";
            lora_->send(pkt.serialize());
        }
    }

    telemetry::NodeConfig config_;
    std::shared_ptr<telemetry::TelegramClient> client_;
    std::shared_ptr<telemetry::OfflineQueue> queue_;
    std::shared_ptr<telemetry::LoRaDriver> lora_;
};

} // namespace guardian
