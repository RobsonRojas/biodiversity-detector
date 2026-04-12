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

#include <vector>
#include <map>
#include <cmath>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace guardian::telemetry {

/**
 * @struct NodeCoords
 * @brief Geographic or relative spatial coordinates
 */
struct NodeCoords {
    double lat;
    double lon;
    double accuracy; // in meters (radius)
    uint32_t last_update;
};

/**
 * @class LocalizationEngine
 * @brief Handles iterative multi-lateration and distance estimation from RSSI
 */
class LocalizationEngine {
public:
    LocalizationEngine();
    
    /**
     * @brief Update the node's position estimate based on neighbor beacons
     * @param neighbor_id ID of the neighbor node
     * @param lat Latitude of the neighbor
     * @param lon Longitude of the neighbor
     * @param accuracy Accuracy of the neighbor's coordinate
     * @param rssi RSSI of the neighbor's beacon
     */
    void update_neighbor(uint16_t neighbor_id, double lat, double lon, double accuracy, int rssi);
    
    /**
     * @brief Run the multi-lateration calculation
     * @return true if position was updated with higher confidence
     */
    bool calculate_position();
    
    /**
     * @brief Get the current best estimate of the node's position
     */
    NodeCoords get_current_coords() const { return current_coords_; }
    
    /**
     * @brief Set the initial anchor position (e.g. if this is the gateway)
     */
    void set_anchor(double lat, double lon);

private:
    NodeCoords current_coords_;
    struct Anchor {
        NodeCoords coords;
        double distance_m;
    };
    std::map<uint16_t, Anchor> anchors_;
    
    // Path Loss Model Parameters (tuned for jungle)
    const double N_FACTOR = 3.5; // Path loss exponent (3-4 for dense foliage)
    const double REFERENCE_RSSI = -45.0; // RSSI at 1 meter
    
    double rssi_to_distance(int rssi);
    double calculate_error(double lat, double lon, const std::vector<Anchor>& test_anchors);
};

} // namespace guardian::telemetry
