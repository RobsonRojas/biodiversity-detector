#pragma once
#include "../utils/compat.hpp"

#include "MeshProtocol.hpp"
#include "LoRaDriver.hpp"
#include <string>
#include <memory>

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
    std::shared_ptr<LoRaDriver> driver;
    
    NodeConfig() : node_id(0), role(NodeRole::Leaf), route_manager(0) {}
    NodeConfig(uint16_t id) : node_id(id), role(NodeRole::Leaf), route_manager(id) {}
};

class ConfigParser {
public:
    static NodeConfig parse_from_env();
    static NodeConfig parse_static_config(const std::string& config_path);
};

} // namespace guardian::telemetry
