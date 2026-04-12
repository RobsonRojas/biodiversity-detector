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

#include "LocalizationEngine.hpp"
#include <algorithm>
#include <vector>
#include <cmath>
#include <iostream>

namespace guardian::telemetry {

LocalizationEngine::LocalizationEngine() : current_coords_{0.0, 0.0, 100.0, 0} {}

void LocalizationEngine::update_neighbor(uint16_t neighbor_id, double lat, double lon, double accuracy, int rssi) {
    if (accuracy < 0.1) { // 0 accuracy means not set
        return;
    }
    
    Anchor anchor;
    anchor.coords.lat = lat;
    anchor.coords.lon = lon;
    anchor.coords.accuracy = accuracy;
    anchor.distance_m = rssi_to_distance(rssi);
    
    anchors_[neighbor_id] = anchor;
}

void LocalizationEngine::set_anchor(double lat, double lon) {
    current_coords_.lat = lat;
    current_coords_.lon = lon;
    current_coords_.accuracy = 1.0; 
}

bool LocalizationEngine::calculate_position() {
    if (anchors_.size() < 3) {
        return false;
    }
    
    std::vector<Anchor> test_anchors;
    for (auto const& [id, anchor] : anchors_) {
        test_anchors.push_back(anchor);
    }
    
    // Initial guess: Centroid
    double current_lat = 0, current_lon = 0;
    for (const auto& anchor : test_anchors) {
        current_lat += anchor.coords.lat;
        current_lon += anchor.coords.lon;
    }
    current_lat /= test_anchors.size();
    current_lon /= test_anchors.size();
    
    // Iterative LSE (Least Squares Estimation)
    double step_size = 0.0001; 
    const int iterations = 100;
    
    for (int i = 0; i < iterations; i++) {
        double current_error = calculate_error(current_lat, current_lon, test_anchors);
        
        double d_lat = (calculate_error(current_lat + step_size, current_lon, test_anchors) - current_error) / step_size;
        double d_lon = (calculate_error(current_lat, current_lon + step_size, test_anchors) - current_error) / step_size;
        
        current_lat -= d_lat * step_size;
        current_lon -= d_lon * step_size;
        
        step_size *= 0.95; 
    }
    
    double final_error = calculate_error(current_lat, current_lon, test_anchors);
    if (final_error < current_coords_.accuracy || current_coords_.accuracy >= 100) {
        current_coords_.lat = current_lat;
        current_coords_.lon = current_lon;
        current_coords_.accuracy = final_error;
        std::cout << "[Localization] Position Updated: " << current_lat << ", " << current_lon 
                  << " (Err: " << (int)final_error << "m)" << std::endl;
        return true;
    }
    
    return false;
}

double LocalizationEngine::rssi_to_distance(int rssi) {
    double ratio = (double)(REFERENCE_RSSI - (double)rssi) / (10.0 * N_FACTOR);
    return std::pow(10, ratio);
}

double LocalizationEngine::calculate_error(double lat, double lon, const std::vector<Anchor>& test_anchors) {
    double total_error = 0;
    for (const auto& anchor : test_anchors) {
        double d_lat = (lat - anchor.coords.lat) * 111320.0; 
        double d_lon = (lon - anchor.coords.lon) * 111320.0 * cos(lat * M_PI / 180.0);
        double dist = sqrt(d_lat*d_lat + d_lon*d_lon);
        
        double residual = dist - anchor.distance_m;
        total_error += residual * residual;
    }
    return sqrt(total_error / test_anchors.size());
}

} // namespace guardian::telemetry
