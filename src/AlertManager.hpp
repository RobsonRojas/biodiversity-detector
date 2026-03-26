#pragma once

#include "telemetry/TelegramClient.hpp"
#include "telemetry/AlertFormatter.hpp"
#include "telemetry/OfflineQueue.hpp"
#include "telemetry/LoRaDriver.hpp"
#include "telemetry/ConfigParser.hpp"
#include "telemetry/MeshProtocol.hpp"
#include "telemetry/SupabaseClient.hpp"
#include "utils/compat.hpp"
#include <memory>
#include <iostream>

namespace guardian {

class AlertManager {
public:
    AlertManager(const telemetry::NodeConfig& config,
                 std::shared_ptr<telemetry::TelegramClient> client,
                 std::shared_ptr<telemetry::OfflineQueue> queue,
                 std::shared_ptr<telemetry::LoRaDriver> lora,
                 std::shared_ptr<telemetry::SupabaseClient> supabase)
        : config_(config), client_(std::move(client)), queue_(std::move(queue)), 
          lora_(std::move(lora)), supabase_(std::move(supabase)) {}

    void on_detection(const telemetry::DetectionEvent& event) {
        if (event.confidence < 0.85f) {
            return;
        }

        std::string message = telemetry::AlertFormatter::format_detection(event);
        
        if (config_.role == telemetry::NodeRole::Gateway) {
            send_to_telegram(message);
            send_to_supabase(event.device_id, event.confidence);
        } else {
            send_via_mesh(message);
        }
    }
    
    void on_mesh_receive(std::span<const uint8_t> data) {
        auto pkt = telemetry::MeshPacket::deserialize(data);
        if (pkt && pkt->verify_signature()) {
            if (pkt->header.sender_id == config_.node_id) return; // Ignore self

            std::cout << "[Mesh] RECV: sender=0x" << std::hex << pkt->header.sender_id 
                      << " target=0x" << pkt->header.target_id 
                      << " prev=0x" << pkt->header.prev_hop_id
                      << " hops_rem=" << std::dec << (int)pkt->header.hop_limit << std::endl;

            if (pkt->header.target_id == config_.node_id) {
                if (config_.role == telemetry::NodeRole::Gateway) {
                    std::string msg(pkt->payload.begin(), pkt->payload.end());
                    std::cout << "[Gateway] Final destination reached. Forwarding to external sinks.\n";
                    send_to_telegram(msg);
                    send_to_supabase(std::to_string(pkt->header.sender_id), 0.95); 
                } else {
                    std::cout << "[Node] Packet reached target directly.\n";
                }
            } else if (config_.role == telemetry::NodeRole::Router || config_.role == telemetry::NodeRole::Gateway) {
                if (pkt->header.hop_limit > 0) {
                    pkt->header.hop_limit--;
                    auto next_hop = config_.route_manager.get_next_hop(pkt->header.target_id);
                    
                    if (next_hop && *next_hop == pkt->header.prev_hop_id) {
                         std::cout << "[Mesh] BLOCK: Avoiding send back to 0x" << std::hex << *next_hop << std::dec << std::endl;
                         return;
                    }

                    if (next_hop && lora_) {
                        std::cout << "[Mesh] FW: next_hop=0x" << std::hex << *next_hop << std::dec << std::endl;
                        pkt->header.prev_hop_id = config_.node_id; // Set self as previous hop
                        lora_->send(pkt->serialize());
                    } else {
                        std::cout << "[Mesh] DROP: No route to 0x" << std::hex << pkt->header.target_id << std::dec << std::endl;
                    }
                } else {
                    std::cout << "[Mesh] DROP: Hop limit exceeded.\n";
                }
            }
        }
    }

    void send_heartbeat() {
        if (!lora_) return;
        const uint16_t gateway_id = 0x0005; // Gateway in our 5-node simulation
        std::string hb = "HEARTBEAT from 0x" + std::to_string(config_.node_id);
        
        telemetry::MeshPacket pkt;
        pkt.header.target_id = gateway_id;
        pkt.header.sender_id = config_.node_id;
        pkt.header.prev_hop_id = config_.node_id;
        pkt.header.hop_limit = 10;
        pkt.header.payload_len = hb.size();
        pkt.header.signature = 0xABCD;
        pkt.payload.assign(hb.begin(), hb.end());

        auto next_hop = config_.route_manager.get_next_hop(gateway_id);
        if (next_hop) {
            std::cout << "[Mesh] Heartbeat -> 0x" << std::hex << *next_hop << std::dec << std::endl;
            lora_->send(pkt.serialize());
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
        const uint16_t gateway_id = 0x0005; // Gateway in our 5-node simulation
        telemetry::MeshPacket pkt;
        pkt.header.target_id = gateway_id;
        pkt.header.sender_id = config_.node_id;
        pkt.header.prev_hop_id = config_.node_id;
        pkt.header.hop_limit = 10;
        pkt.header.payload_len = message.size();
        pkt.header.signature = 0xABCD;
        pkt.payload.assign(message.begin(), message.end());
        
        auto next_hop = config_.route_manager.get_next_hop(pkt.header.target_id);
        if (next_hop) {
            std::cout << "[Leaf/Router] Sending detection alert via Mesh to hop " << *next_hop << "\n";
            lora_->send(pkt.serialize());
        }
    }

    void send_to_supabase(const std::string& node_id, double confidence) {
        if (!supabase_) return;
        supabase_->post_detection(node_id, confidence);
    }

    telemetry::NodeConfig config_;
    std::shared_ptr<telemetry::TelegramClient> client_;
    std::shared_ptr<telemetry::OfflineQueue> queue_;
    std::shared_ptr<telemetry::LoRaDriver> lora_;
    std::shared_ptr<telemetry::SupabaseClient> supabase_;
};

} // namespace guardian
