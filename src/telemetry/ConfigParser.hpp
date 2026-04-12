/*
 * Copyright (C) 2026 Robson Rojas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
