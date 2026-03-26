#include "ConfigParser.hpp"
#include <iostream>

namespace guardian::telemetry {

NodeConfig ConfigParser::parse_static_config(const std::string& config_path) {
    // Simulated static configuration parser
    std::cout << "[Config] Loading static topology..." << std::endl;
    NodeConfig config(0x1001); // Local Node ID
    
    config.role = NodeRole::Router;
    // Add static route: dest -> next_hop
    config.route_manager.add_route(0x0001, 0x1002);
    
    return config;
}

} // namespace guardian::telemetry
