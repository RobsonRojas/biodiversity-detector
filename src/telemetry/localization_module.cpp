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

#include "localization_module.hpp"
#include <algorithm>
#include <vector>

/**
 * @brief Initialize Localization Engine
 */
LocalizationEngine::LocalizationEngine() : current_coords_{0.0, 0.0, 1000.0, 0, 255} {}

/**
 * @brief Update the node's position estimate based on neighbor beacons
 * @param neighbor_id ID of the neighbor node
 * @param coords Coordinates reported by the neighbor
 * @param rssi RSSI of the neighbor's beacon
 */
void LocalizationEngine::update_neighbor(uint8_t neighbor_id, const NodeCoords& coords, int rssi) {
    if (coords.accuracy < 1.0) { // Invalid coordinates
        return;
    }
    
    Anchor anchor;
    anchor.coords = coords;
    anchor.distance_m = rssi_to_distance(rssi);
    
    anchors_[neighbor_id] = anchor;
}

/**
 * @brief Set the initial anchor position (e.g. if this is the gateway)
 */
void LocalizationEngine::set_anchor(double lat, double lon) {
    current_coords_.lat = lat;
    current_coords_.lon = lon;
    current_coords_.accuracy = 1.0; // 1 meter precision for gateway-anchor
    current_coords_.hop_count = 0;   // Gateway is hop 0
}

/**
 * @brief Run the multi-lateration calculation
 * @return true if position was updated with higher confidence
 */
bool LocalizationEngine::calculate_position() {
    if (anchors_.size() < 3) {
        return false;
    }
    
    // Convert anchors to a simple list
    std::vector<Anchor> test_anchors;
    for (auto const& [id, anchor] : anchors_) {
        test_anchors.push_back(anchor);
    }
    
    // Initial guess: Centroid of all anchors
    double current_lat = 0, current_lon = 0;
    for (const auto& anchor : test_anchors) {
        current_lat += anchor.coords.lat;
        current_lon += anchor.coords.lon;
    }
    current_lat /= test_anchors.size();
    current_lon /= test_anchors.size();
    
    // Simple Gradient Descent / Iterative Trilateration
    // For production, a more robust least squares solver might be used.
    double step_size = 0.0001; // Approx 11m in Lat/Lon at equator
    const int iterations = 100;
    
    for (int i = 0; i < iterations; i++) {
        double current_error = calculate_error(current_lat, current_lon, test_anchors);
        
        // Compute gradients in both lat and lon directions
        double d_lat = (calculate_error(current_lat + step_size, current_lon, test_anchors) - current_error) / step_size;
        double d_lon = (calculate_error(current_lat, current_lon + step_size, test_anchors) - current_error) / step_size;
        
        current_lat -= d_lat * step_size;
        current_lon -= d_lon * step_size;
        
        step_size *= 0.95; // Annealing step size
    }
    
    // Final Confidence Check
    double final_error = calculate_error(current_lat, current_lon, test_anchors);
    if (final_error < current_coords_.accuracy || current_coords_.accuracy >= 100) {
        current_coords_.lat = current_lat;
        current_coords_.lon = current_lon;
        current_coords_.accuracy = final_error;
        
        // Propagate hop count: min(neighbor_hops) + 1
        uint8_t min_hops = 255;
        for (const auto& anchor : test_anchors) {
            if (anchor.coords.hop_count < min_hops) {
                min_hops = anchor.coords.hop_count;
            }
        }
        current_coords_.hop_count = (min_hops < 255) ? (min_hops + 1) : 255;
        
        return true;
    }
    
    return false;
}

/**
 * @brief Log-Distance Path Loss Model implementation
 */
double LocalizationEngine::rssi_to_distance(int rssi) {
    // Model: RSSI = P0 - 10 * N * log10(d)
    // Distance d = 10^((P0 - RSSI) / (10 * N))
    double ratio = (double)(REFERENCE_RSSI - (double)rssi) / (10.0 * N_FACTOR);
    return pow(10, ratio);
}

/**
 * @brief Sum of squared residuals for trilateration (Weighted Least Squares)
 */
double LocalizationEngine::calculate_error(double lat, double lon, const std::vector<Anchor>& test_anchors) {
    double total_weighted_error = 0;
    double total_weight = 0;

    for (const auto& anchor : test_anchors) {
        double weight = calculate_weight(anchor);
        
        // Simple Euclidean distance for lat/lon as nodes are close (~50-200m)
        double d_lat = (lat - anchor.coords.lat) * 111320.0; // Approx m/deg
        double d_lon = (lon - anchor.coords.lon) * 111320.0 * cos(lat * M_PI / 180.0);
        double dist = sqrt(d_lat*d_lat + d_lon*d_lon);
        
        double residual = dist - anchor.distance_m;
        total_weighted_error += weight * (residual * residual);
        total_weight += weight;
    }
    
    return sqrt(total_weighted_error / total_weight);
}

/**
 * @brief Calculate anchor weight based on its reported accuracy and hop count
 */
double LocalizationEngine::calculate_weight(const Anchor& anchor) {
    // Weight is inversely proportional to accuracy and hop count
    // Lower accuracy (higher meters) and higher hop count => lower weight
    double accuracy_weight = 1.0 / (anchor.coords.accuracy + 1.0);
    double hop_penalty = 1.0 / (double)(anchor.coords.hop_count + 1);
    return accuracy_weight * hop_penalty;
}
