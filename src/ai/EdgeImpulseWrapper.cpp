#include "../utils/compat.hpp"
#include "EdgeImpulseWrapper.hpp"
#include <unistd.h>

#ifdef EDGE_IMPULSE_ENABLED
// #include "edge-impulse-sdk/classifier/ei_classifier_porting.h"
// #include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#endif

namespace guardian::ai {

EdgeImpulseWrapper::EdgeImpulseWrapper() {}

bool EdgeImpulseWrapper::init() {
    // Stage 3 Initialization: Load model pointers
    return true;
}

std::vector<DetectionResult> EdgeImpulseWrapper::classify(guardian::span<const int16_t> samples) {
    std::vector<DetectionResult> results;
    
    // 1. Feature Extraction (Stage 3 preprocessing)
    auto features = extract_features(samples.data(), samples.size());

    // 2. Run Inference
#ifdef EDGE_IMPULSE_ENABLED
    // ei_impulse_result_t result = { 0 };
    // signal_t signal;
    // signal.total_length = count;
    // signal.get_data = &raw_feature_get_data;
    // run_classifier(&signal, &result, false);
    // ... parse results ...
#else
    // Mock inference with node-specific simulator triggers
    const char* node_id = std::getenv("NODE_ID");
    std::string id_str = node_id ? node_id : "unknown";
    std::string chainsaw_trig = "/tmp/" + id_str + "_chainsaw_trigger";
    std::string frog_trig = "/tmp/" + id_str + "_frog_trigger";

    if (::access(chainsaw_trig.c_str(), F_OK) == 0) {
        results.push_back({"Chainsaw", 0.98f});
        ::unlink(chainsaw_trig.c_str());
    } else if (::access(frog_trig.c_str(), F_OK) == 0) {
        results.push_back({"frog:Boana geographica", 0.95f});
        ::unlink(frog_trig.c_str());
    } else {
        results.push_back({"Background", 0.05f});
    }
#endif

    return results;
}

std::vector<float> EdgeImpulseWrapper::extract_features(const int16_t* samples, size_t count) {
    std::vector<float> features;
    // Implementation of Log-Mel Spectrogram extraction would go here
    // For now, mirroring the task 3.2 requirement
    return features;
}

} // namespace guardian::ai
