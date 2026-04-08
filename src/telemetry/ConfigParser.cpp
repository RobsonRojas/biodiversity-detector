#include "../utils/compat.hpp"
#include "ConfigParser.hpp"
#include "SimulatedLoRaDriver.hpp"
#include "PhysicalLoRaDriver.hpp"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>

namespace guardian::telemetry {

NodeConfig ConfigParser::parse_from_env() {
    NodeConfig config(0);

    // 1. Basic Identifiers
    const char* node_id_env = getenv("NODE_ID");
    config.node_id = node_id_env ? static_cast<uint16_t>(std::stoul(node_id_env, nullptr, 0)) : 0x0001;

    const char* role_env = getenv("NODE_ROLE");
    if (role_env && (std::string(role_env) == "Gateway" || std::string(role_env) == "gateway")) {
        config.role = NodeRole::Gateway;
    } else if (role_env && (std::string(role_env) == "Router" || std::string(role_env) == "router")) {
        config.role = NodeRole::Router;
    } else {
        config.role = NodeRole::Leaf;
    }
    
    std::cout << "[SYSTEM] Config: NodeID=0x" << std::hex << config.node_id 
              << ", Role=" << (config.role == NodeRole::Gateway ? "Gateway" : 
                             (config.role == NodeRole::Router ? "Router" : "Leaf")) 
              << std::dec << std::endl;

    // 2. Mesh Routing Table
    const char* routes_env = getenv("MESH_ROUTES"); // format: "target:next,target:next"
    if (routes_env) {
        std::stringstream ss(routes_env);
        std::string route_str;
        while (std::getline(ss, route_str, ',')) {
            size_t colon = route_str.find(':');
            if (colon != std::string::npos) {
                uint16_t dest = static_cast<uint16_t>(std::stoul(route_str.substr(0, colon), nullptr, 0));
                uint16_t next = static_cast<uint16_t>(std::stoul(route_str.substr(colon + 1), nullptr, 0));
                config.route_manager.add_route(dest, next);
            }
        }
    }

    // 3. Driver Initialization
    const char* lora_driver_env = getenv("LORA_DRIVER");
    if (lora_driver_env && std::string(lora_driver_env) == "physical") {
        const char* spi_dev = getenv("LORA_SPI") ? getenv("LORA_SPI") : "/dev/spidev0.0";
        config.driver = std::make_shared<PhysicalLoRaDriver>(spi_dev, 8, 24, 25, 17); 
    } else {
        uint16_t sim_port = getenv("SIM_LORA_PORT") ? static_cast<uint16_t>(std::stoul(getenv("SIM_LORA_PORT"))) : 5000;
        std::vector<std::string> neighbors;
        const char* neigh_env = getenv("SIM_LORA_NEIGHBORS");
        if (neigh_env) {
            std::stringstream ss(neigh_env);
            std::string n;
            while (std::getline(ss, n, ',')) {
                if (!n.empty()) neighbors.push_back(n);
            }
        }
        config.driver = std::make_shared<SimulatedLoRaDriver>(sim_port, neighbors);
    }

    return config;
}

} // namespace guardian::telemetry
