#include "../../src/esp32-s3-cpp/main/localization_module.hpp"
#include <iostream>
#include <cassert>
#include <vector>

void test_rssi_to_distance() {
    LocalizationEngine engine;
    
    // -45 dBm should be 1m
    double d1 = engine.rssi_to_distance(-45);
    std::cout << "RSSI -45dBm -> " << d1 << "m" << std::endl;
    assert(std::abs(d1 - 1.0) < 0.1);
    
    // -80 dBm should be > 1m (approx 10^((45-80)/(10*3.5)) = 10^(35/35) = 10m)
    double d10 = engine.rssi_to_distance(-80);
    std::cout << "RSSI -80dBm -> " << d10 << "m" << std::endl;
    assert(std::abs(d10 - 10.0) < 1.0);
}

void test_trilateration() {
    LocalizationEngine engine;
    
    // Setup 3 anchors in a triangle
    // A: (0,0), B: (0, 0.001), C: (0.001, 0)
    NodeCoords a = {0.0, 0.0, 1.0, 0};
    NodeCoords b = {0.0, 0.001, 1.0, 0};
    NodeCoords c = {0.001, 0.0, 1.0, 0};
    
    // Target is at (0.0005, 0.0005)
    // Distance to each is sqrt(0.0005^2 + 0.0005^2) * 111320 = 78.7m
    // RSSI for 78.7m = -45 - 10 * 3.5 * log10(78.7) = -111 dBm
    
    engine.update_neighbor(1, a, -111);
    engine.update_neighbor(2, b, -111);
    engine.update_neighbor(3, c, -111);
    
    bool success = engine.calculate_position();
    NodeCoords result = engine.get_current_coords();
    
    std::cout << "Trilateration Result: (" << result.lat << ", " << result.lon << ") Accuracy: " << result.accuracy << std::endl;
    
    assert(success);
    assert(std::abs(result.lat - 0.0005) < 0.0002);
    assert(std::abs(result.lon - 0.0005) < 0.0002);
}

int main() {
    try {
        test_rssi_to_distance();
        test_trilateration();
        std::cout << "ALL TESTS PASSED!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
