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
            send_to_supabase(event.device_id, event.confidence, 4200, -85); // Default mock for Gateway self-report
        } else {
            send_via_mesh(message, 4100, -90); // Default mock for Leaf/Router
        }
    }
    
    void on_mesh_receive(std::span<const uint8_t> data) {
        auto pkt = telemetry::MeshPacket::deserialize(data);
        if (pkt && pkt->verify_signature()) {
            if (pkt->header.sender_id == config_.node_id) return; // Ignore self

            std::cout << "[Mesh] RECV: sender=0x" << std::hex << pkt->header.sender_id 
                      << " target=0x" << pkt->header.target_id 
                      << " bat=" << std::dec << pkt->header.battery_mv << "mV"
                      << " rssi=" << (int)pkt->header.last_rssi << "dBm"
                      << " hops_rem=" << (int)pkt->header.hop_limit << std::endl;

            if (pkt->header.target_id == config_.node_id) {
                if (config_.role == telemetry::NodeRole::Gateway) {
                    std::string msg(pkt->payload.begin(), pkt->payload.end());
                    std::cout << "[Gateway] Final destination reached. Reporting Telemetry.\n";
                    send_to_telegram(msg + " (Bat: " + std::to_string(pkt->header.battery_mv) + "mV)");
                    send_to_supabase(std::to_string(pkt->header.sender_id), 0.95, pkt->header.battery_mv, pkt->header.last_rssi); 
                }
            } else if (config_.role == telemetry::NodeRole::Router || config_.role == telemetry::NodeRole::Gateway) {
                if (pkt->header.hop_limit > 0) {
                    pkt->header.hop_limit--;
                    auto next_hop = config_.route_manager.get_next_hop(pkt->header.target_id);
                    
                    if (next_hop) {
                        std::cout << "[Mesh] Routing to 0x" << std::hex << pkt->header.target_id 
                                  << " via next_hop 0x" << *next_hop 
                                  << " (prev: 0x" << pkt->header.prev_hop_id << ")" << std::dec << std::endl;
                        
                        if (*next_hop == pkt->header.prev_hop_id) {
                            std::cout << "[Mesh] Split-horizon drop.\n";
                            return;
                        }

                        if (lora_) {
                            pkt->header.prev_hop_id = config_.node_id;
                            lora_->send(pkt->serialize());
                        }
                    } else {
                        std::cout << "[Mesh] No route for 0x" << std::hex << pkt->header.target_id << std::dec << std::endl;
                    }
                }
            }
        }
    }

    void send_heartbeat(uint16_t battery_mv, int8_t rssi) {
        if (!lora_) return;
        const uint16_t gateway_id = 0x0005;
        std::string hb = "HEARTBEAT";
        
        telemetry::MeshPacket pkt;
        pkt.header.target_id = gateway_id;
        pkt.header.sender_id = config_.node_id;
        pkt.header.prev_hop_id = config_.node_id;
        pkt.header.battery_mv = battery_mv;
        pkt.header.last_rssi = rssi;
        pkt.header.hop_limit = 10;
        pkt.header.payload_len = hb.size();
        pkt.header.signature = 0xABCD;
        pkt.payload.assign(hb.begin(), hb.end());

        auto next_hop = config_.route_manager.get_next_hop(gateway_id);
        if (next_hop) {
            lora_->send(pkt.serialize());
        }
    }

private:
    void send_to_telegram(const std::string& message) {
        if (!client_) return;
        client_->send_message(message);
    }

    void send_via_mesh(const std::string& message, uint16_t battery_mv, int8_t rssi) {
        if (!lora_) return;
        const uint16_t gateway_id = 0x0005;
        telemetry::MeshPacket pkt;
        pkt.header.target_id = gateway_id;
        pkt.header.sender_id = config_.node_id;
        pkt.header.prev_hop_id = config_.node_id;
        pkt.header.battery_mv = battery_mv;
        pkt.header.last_rssi = rssi;
        pkt.header.hop_limit = 10;
        pkt.header.payload_len = message.size();
        pkt.header.signature = 0xABCD;
        pkt.payload.assign(message.begin(), message.end());
        
        auto next_hop = config_.route_manager.get_next_hop(pkt.header.target_id);
        if (next_hop) {
            lora_->send(pkt.serialize());
        }
    }

    void send_to_supabase(const std::string& node_id, double confidence, uint16_t battery_mv, int8_t rssi) {
        if (!supabase_) return;
        supabase_->post_detection(node_id, confidence);
        // In a real implementation, we'd add battery/rssi to the post_detection call
        std::cout << "[Supabase] Syncing Telemetry: Node=" << node_id << " Bat=" << battery_mv << " RSSI=" << (int)rssi << "\n";
    }

    telemetry::NodeConfig config_;
    std::shared_ptr<telemetry::TelegramClient> client_;
    std::shared_ptr<telemetry::OfflineQueue> queue_;
    std::shared_ptr<telemetry::LoRaDriver> lora_;
    std::shared_ptr<telemetry::SupabaseClient> supabase_;
};

} // namespace guardian
