#pragma once

#include "MeshProtocol.hpp"
#include <string>

namespace guardian::telemetry {

enum class NodeRole {
    Leaf,
    Router,
    Gateway
};

struct NodeConfig {
    uint16_t node_id;
    NodeRole role;
    RouteManager route_manager;
    
    NodeConfig(uint16_t id) : node_id(id), role(NodeRole::Leaf), route_manager(id) {}
};

class ConfigParser {
public:
    static NodeConfig parse_static_config(const std::string& config_path);
};

} // namespace guardian::telemetry
