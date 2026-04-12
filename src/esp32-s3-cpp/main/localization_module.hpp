#pragma once

#include <vector>
#include <map>
#include <cmath>
#include <stdint.h>

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
     * @param coords Coordinates reported by the neighbor
     * @param rssi RSSI of the neighbor's beacon
     */
    void update_neighbor(uint8_t neighbor_id, const NodeCoords& coords, int rssi);
    
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
    std::map<uint8_t, Anchor> anchors_;
    
    // Path Loss Model Parameters (tuned for jungle)
    const double N_FACTOR = 3.5; // Path loss exponent (3-4 for dense foliage)
    const double REFERENCE_RSSI = -45.0; // RSSI at 1 meter
    
    double rssi_to_distance(int rssi);
    double calculate_error(double x, double y, const std::vector<Anchor>& test_anchors);
};
