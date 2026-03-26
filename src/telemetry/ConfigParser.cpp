#include "ConfigParser.hpp"
#include <iostream>
#include <cstdlib>
#include <sstream>

namespace guardian::telemetry {

NodeConfig ConfigParser::parse_static_config(const std::string& config_path) {
    (void)config_path; // Unused for now as we use env vars

    const char* id_env = std::getenv("NODE_ID");
    uint16_t id = id_env ? static_cast<uint16_t>(std::stoul(id_env, nullptr, 0)) : 0x1001;
    
    NodeConfig config(id);
    
    const char* role_env = std::getenv("NODE_ROLE");
    if (role_env) {
        std::string role(role_env);
        if (role == "Gateway") config.role = NodeRole::Gateway;
        else if (role == "Router") config.role = NodeRole::Router;
        else config.role = NodeRole::Leaf;
    }

    const char* routes_env = std::getenv("STATIC_ROUTES");
    if (routes_env) {
        std::stringstream ss(routes_env);
        std::string route_str;
        while (std::getline(ss, route_str, ',')) {
            size_t colon = route_str.find(':');
            if (colon != std::string::npos) {
                uint16_t dest = static_cast<uint16_t>(std::stoul(route_str.substr(0, colon), nullptr, 0));
                uint16_t next = static_cast<uint16_t>(std::stoul(route_str.substr(colon + 1), nullptr, 0));
                config.route_manager.add_route(dest, next);
                std::cout << "[Config] Added route: " << std::hex << dest << " -> " << next << std::dec << std::endl;
            }
        }
    }
    
    std::cout << "[Config] Node " << std::hex << id << std::dec << " initialized as " 
              << (config.role == NodeRole::Gateway ? "Gateway" : (config.role == NodeRole::Router ? "Router" : "Leaf")) 
              << std::endl;

    return config;
}

} // namespace guardian::telemetry
