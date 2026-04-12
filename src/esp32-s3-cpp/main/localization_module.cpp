#include "localization_module.hpp"
#include <algorithm>
#include <vector>

/**
 * @brief Initialize Localization Engine
 */
LocalizationEngine::LocalizationEngine() : current_coords_{0,0,100,0} {}

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
 * @brief Sum of squared residuals for trilateration
 */
double LocalizationEngine::calculate_error(double lat, double lon, const std::vector<Anchor>& test_anchors) {
    double total_error = 0;
    for (const auto& anchor : test_anchors) {
        // Simple Euclidean distance for lat/lon as nodes are close (~50-200m)
        // 0.00001 degrees is ~1.1 meters
        double d_lat = (lat - anchor.coords.lat) * 111320.0; // Approx m/deg
        double d_lon = (lon - anchor.coords.lon) * 111320.0 * cos(lat * M_PI / 180.0);
        double dist = sqrt(d_lat*d_lat + d_lon*d_lon);
        
        double residual = dist - anchor.distance_m;
        total_error += residual * residual;
    }
    return sqrt(total_error / test_anchors.size());
}
