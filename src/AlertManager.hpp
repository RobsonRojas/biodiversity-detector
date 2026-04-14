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
#include "utils/json.hpp"
#include <map>
#include <vector>
#include <chrono>
#include "telemetry/localization_module.hpp"

namespace guardian {

class AlertManager {
public:
    AlertManager(const telemetry::NodeConfig& config,
                 std::shared_ptr<telemetry::TelegramClient> client,
                 std::shared_ptr<telemetry::OfflineQueue> queue,
                 std::shared_ptr<telemetry::LoRaDriver> lora,
                 std::shared_ptr<telemetry::SupabaseClient> supabase,
                 LocalizationEngine* localization = nullptr)
        : config_(config), client_(std::move(client)), queue_(std::move(queue)), 
          lora_(std::move(lora)), supabase_(std::move(supabase)), 
          localization_(localization) {}

    void on_detection(const telemetry::DetectionEvent& event) {
        if (event.confidence < 0.85f) {
            return;
        }

        std::string message = telemetry::AlertFormatter::format_detection(event);
        
        if (config_.role == telemetry::NodeRole::Gateway) {
            send_to_telegram(message);
            send_to_supabase(event.device_id, event.confidence, 4200, -85); 
        } else {
            send_via_mesh(message, 4100, -90); 
        }
    }
    
    void on_mesh_receive(guardian::span<const uint8_t> data) {
        auto pkt = telemetry::MeshPacket::deserialize(data);
        if (pkt && pkt->verify_signature()) {
            if (pkt->header.sender_id == config_.node_id) return; 

            // Handle detection correlation if it's an alert/detection
            std::string msg(pkt->payload.begin(), pkt->payload.end());
            if (msg.find("ALERT:") == 0) {
                handle_detection_report(*pkt, msg);
            }
            
            // Handle Localization Beacons
            if (msg.find("BEACON:") == 0 && localization_) {
                // BEACON:ID:LAT:LON:ACC:HOPS
                size_t first = msg.find(':') + 1;
                size_t second = msg.find(':', first);
                size_t third = msg.find(':', second + 1);
                size_t fourth = msg.find(':', third + 1);
                size_t fifth = msg.find(':', fourth + 1);

                if (fifth != std::string::npos) {
                    uint8_t id = std::stoi(msg.substr(first, second - first));
                    NodeCoords coords;
                    coords.lat = std::stod(msg.substr(second + 1, third - second - 1));
                    coords.lon = std::stod(msg.substr(third + 1, fourth - third - 1));
                    coords.accuracy = std::stod(msg.substr(fourth + 1, fifth - fourth - 1));
                    coords.hop_count = std::stoi(msg.substr(fifth + 1));
                    
                    localization_->update_neighbor(id, coords, pkt->header.last_rssi);
                    if (localization_->calculate_position()) {
                        std::cout << "[Localization] Node position updated! New acc: " << localization_->get_current_coords().accuracy << "m\n";
                    }
                }
            }

            if (pkt->payload.size() >= 186) {
                handle_audio_fragment(*pkt);
                return;
            }

            std::cout << "[Mesh] RECV: sender=0x" << std::hex << pkt->header.sender_id 
                      << " target=0x" << pkt->header.target_id 
                      << " bat=" << std::dec << pkt->header.battery_mv << "mV"
                      << " rssi=" << (int)pkt->header.last_rssi << "dBm"
                      << " pos=(" << pkt->header.lat << ", " << pkt->header.lon << ")"
                      << " hops_rem=" << (int)pkt->header.hop_limit << std::endl;

            if (pkt->header.target_id == config_.node_id) {
                if (config_.role == telemetry::NodeRole::Gateway) {
                    std::cout << "[Gateway] Final destination reached. Reporting Telemetry.\n";
                    
                    std::string sound_class = "chainsaw"; 
                    if (msg.find("Boana") != std::string::npos) sound_class = "frog:Boana geographica";
                    else if (msg.find("birds") != std::string::npos) sound_class = "birds";
                    else if (msg.find("rain") != std::string::npos) sound_class = "rain";

                    std::string map_link = "\n📍 Node Location: https://www.google.com/maps/search/?api=1&query=" + 
                                          std::to_string(pkt->header.lat) + "," + std::to_string(pkt->header.lon);
                    send_to_telegram(msg + " (Bat: " + std::to_string(pkt->header.battery_mv) + "mV)" + map_link);
                    send_to_supabase(std::to_string(pkt->header.sender_id), 0.95, pkt->header.battery_mv, pkt->header.last_rssi, sound_class); 
                }
            } else if (config_.role == telemetry::NodeRole::Router || config_.role == telemetry::NodeRole::Gateway) {
                if (pkt->header.hop_limit > 0) {
                    pkt->header.hop_limit--;
                    auto next_hop = config_.route_manager.get_next_hop(pkt->header.target_id);
                    if (next_hop && lora_) {
                        std::cout << "[Mesh] RELAY: src=0x" << std::hex << pkt->header.sender_id 
                                  << " target=0x" << pkt->header.target_id 
                                  << " via=0x" << *next_hop << std::dec << std::endl;
                        pkt->header.prev_hop_id = config_.node_id;
                        lora_->send(pkt->serialize());
                    } else {
                        std::cout << "[Mesh] DROP: no route to target 0x" << std::hex << pkt->header.target_id << std::dec << std::endl;
                    }
                }
            }
        }
    }

    void send_heartbeat(uint16_t battery_mv, int8_t rssi, const std::string& extra_payload = "") {
        if (!lora_) return;
        const uint16_t gateway_id = 0x0005;
        std::string hb = "HEARTBEAT";
        if (!extra_payload.empty()) {
            hb = extra_payload;
        }
        
        telemetry::MeshPacket pkt;
        pkt.header.target_id = gateway_id;
        pkt.header.sender_id = config_.node_id;
        pkt.header.prev_hop_id = config_.node_id;
        pkt.header.battery_mv = battery_mv;
        pkt.header.last_rssi = rssi;
        pkt.header.hop_limit = 10;
        pkt.header.payload_len = hb.size();
        pkt.header.lat = config_.lat;
        pkt.header.lon = config_.lon;
        
        // If we have an active localization engine, pull the latest coords
        if (localization_) {
            NodeCoords current = localization_->get_current_coords();
            if (current.accuracy < 1000) {
                pkt.header.lat = current.lat;
                pkt.header.lon = current.lon;
            }
        }

        pkt.header.signature = 0xABCD;
        pkt.payload.assign(hb.begin(), hb.end());

        auto next_hop = config_.route_manager.get_next_hop(gateway_id);
        if (next_hop) {
            lora_->send(pkt.serialize());
        }
    }

    LocalizationEngine* get_localization_engine() { return localization_; }
    const telemetry::NodeConfig& get_config() const { return config_; }

private:
    struct DetectionCorrelation {
        uint16_t node_id;
        double lat;
        double lon;
        double confidence;
        std::chrono::steady_clock::time_point timestamp;
    };

    std::map<std::string, std::vector<DetectionCorrelation>> active_detections_;

    void handle_detection_report(const telemetry::MeshPacket& pkt, const std::string& msg) {
        // ALERT:label:conf:LAT:lat:LON:lon
        // Simplified parsing
        size_t l_pos = msg.find("ALERT:") + 6;
        size_t c_pos = msg.find(":", l_pos);
        std::string label = msg.substr(l_pos, c_pos - l_pos);
        
        DetectionCorrelation det;
        det.node_id = pkt.header.sender_id;
        det.lat = pkt.header.lat;
        det.lon = pkt.header.lon;
        det.confidence = 0.9; // Default if not parsed
        det.timestamp = std::chrono::steady_clock::now();

        auto& list = active_detections_[label];
        
        // Remove old detections (> 10s)
        auto now = std::chrono::steady_clock::now();
        list.erase(std::remove_if(list.begin(), list.end(), 
                   [&](const DetectionCorrelation& d) {
                       return std::chrono::duration_cast<std::chrono::seconds>(now - d.timestamp).count() > 10;
                   }), list.end());

        list.push_back(det);

        if (list.size() >= 2) {
            run_acoustic_localization(label, list);
        }
    }

    void run_acoustic_localization(const std::string& label, const std::vector<DetectionCorrelation>& detections) {
        double sum_lat = 0, sum_lon = 0, sum_w = 0;
        for (const auto& d : detections) {
            double w = d.confidence;
            sum_lat += d.lat * w;
            sum_lon += d.lon * w;
            sum_w += w;
        }

        double source_lat = sum_lat / sum_w;
        double source_lon = sum_lon / sum_w;

        std::string alert = "🚨 MULTI-NODE CORRELATION: [" + label + "]\n" +
                            "Estimated Source: https://www.google.com/maps/search/?api=1&query=" + 
                            std::to_string(source_lat) + "," + std::to_string(source_lon) + "\n" +
                            "Nodes reporting: " + std::to_string(detections.size());
        
        send_to_telegram(alert);
        std::cout << "[Gateway] Triangulated " << label << " source at (" << source_lat << ", " << source_lon << ")\n";
    }

    struct AudioSession {
        uint16_t total_frags = 0;
        uint16_t received_count = 0;
        std::string sound_class;
        std::string node_id;
        std::map<uint16_t, std::vector<uint8_t>> fragments;
    };

    std::map<uint16_t, AudioSession> audio_sessions_;

    void handle_audio_fragment(const telemetry::MeshPacket& pkt) {
        if (pkt.payload.size() < 6) return;
        
        uint16_t session_id = (pkt.payload[0] << 8) | pkt.payload[1];
        uint16_t frag_index = (pkt.payload[2] << 8) | pkt.payload[3];
        uint16_t total_frags = (pkt.payload[4] << 8) | pkt.payload[5];

        auto& session = audio_sessions_[session_id];
        if (session.total_frags == 0) {
            session.total_frags = total_frags;
            session.node_id = std::to_string(pkt.header.sender_id);
            session.sound_class = "unknown"; 
            std::cout << "[Gateway] New Audio Session " << session_id << " from Node " << session.node_id << " (Total: " << total_frags << ")\n";
        }

        if (session.fragments.find(frag_index) == session.fragments.end()) {
            std::vector<uint8_t> frag_data(pkt.payload.begin() + 6, pkt.payload.end());
            session.fragments[frag_index] = frag_data;
            session.received_count++;
            
            if (session.received_count % 10 == 0 || session.received_count == total_frags) {
                std::cout << "[Gateway] Session " << session_id << " Progress: " << session.received_count << "/" << total_frags << "\n";
            }

            if (session.received_count >= total_frags) {
                finalize_audio_session(session_id);
            }
        }
    }

    void finalize_audio_session(uint16_t session_id) {
        auto& session = audio_sessions_[session_id];
        std::vector<uint8_t> complete_audio;
        for (uint16_t i = 0; i < session.total_frags; ++i) {
            if (session.fragments.count(i)) {
                complete_audio.insert(complete_audio.end(), session.fragments[i].begin(), session.fragments[i].end());
            } else {
                complete_audio.insert(complete_audio.end(), 180, 0); 
            }
        }

        std::string filename = "audio_" + session.node_id + "_" + std::to_string(session_id) + ".bin";
        if (supabase_) {
            if (supabase_->upload_audio("detection-audio", filename, complete_audio)) {
                std::string url = "/storage/v1/object/public/detection-audio/" + filename;
                supabase_->post_detection(session.node_id, 0.99, "unknown", url);
            }
        }
        if (client_) {
            client_->send_audio("New Sound Detection from Node " + session.node_id, complete_audio);
        }
        
        audio_sessions_.erase(session_id);
    }

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
        pkt.header.lat = config_.lat;
        pkt.header.lon = config_.lon;
        pkt.header.hop_limit = 10;
        pkt.header.payload_len = message.size();
        pkt.header.signature = 0xABCD;
        pkt.payload.assign(message.begin(), message.end());
        
        auto next_hop = config_.route_manager.get_next_hop(pkt.header.target_id);
        if (next_hop) {
            lora_->send(pkt.serialize());
        }
    }

    void send_to_supabase(const std::string& node_id, double confidence, uint16_t battery_mv, int8_t rssi, const std::string& sound_class = "chainsaw") {
        if (!supabase_) return;
        supabase_->post_detection(node_id, confidence, sound_class);
        std::cout << "[Supabase] Syncing Telemetry: Node=" << node_id << " Class=" << sound_class << " Bat=" << battery_mv << " RSSI=" << (int)rssi << "\n";
    }

    telemetry::NodeConfig config_;
    std::shared_ptr<telemetry::TelegramClient> client_;
    std::shared_ptr<telemetry::OfflineQueue> queue_;
    std::shared_ptr<telemetry::LoRaDriver> lora_;
    std::shared_ptr<telemetry::SupabaseClient> supabase_;
    LocalizationEngine* localization_;
};

} // namespace guardian
