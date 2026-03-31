#pragma once

#include <string>
#include <iostream>
#include <curl/curl.h>
#include "../utils/json.hpp"
#include <vector>
#include <cstdint>

namespace guardian {
namespace telemetry {

class SupabaseClient {
public:
    SupabaseClient(const std::string& url, const std::string& key)
        : url_(url), key_(key) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~SupabaseClient() {
        curl_global_cleanup();
    }

    bool post_detection(const std::string& node_id, double confidence, 
                        const std::string& sound_class = "chainsaw",
                        const std::string& audio_url = "") {
        nlohmann::json payload = {
            {"node_id", node_id},
            {"confidence", confidence},
            {"sound_class", sound_class}
        };
        if (!audio_url.empty()) {
            payload["audio_url"] = audio_url;
        }
        return send_post(payload.dump());
    }

    bool post_heartbeat(const std::string& node_id, int battery, int rssi) {
        nlohmann::json payload = {
            {"node_id", node_id},
            {"status", "online"},
            {"battery_mv", battery},
            {"rssi_dbm", rssi}
        };
        return send_post(payload.dump());
    }

    bool upload_audio(const std::string& bucket, const std::string& path, const std::vector<uint8_t>& data) {
        CURL* curl = curl_easy_init();
        if (!curl) return false;

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: audio/wav");
        std::string auth = "Authorization: Bearer " + key_;
        headers = curl_slist_append(headers, auth.c_str());
        std::string api_key = "apikey: " + key_;
        headers = curl_slist_append(headers, api_key.c_str());

        std::string upload_url = url_ + "/storage/v1/object/" + bucket + "/" + path;

        curl_easy_setopt(curl, CURLOPT_URL, upload_url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.data());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());

        CURLcode res = curl_easy_perform(curl);
        bool success = (res == CURLE_OK);

        if (!success) {
            std::cerr << "[Supabase] Audio upload failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "[Supabase] Audio uploaded successfully: " << path << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return success;
    }

private:
    bool send_post(const std::string& data) {
        CURL* curl = curl_easy_init();
        if (!curl) return false;

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string auth = "Authorization: Bearer " + key_;
        headers = curl_slist_append(headers, auth.c_str());
        std::string api_key = "apikey: " + key_;
        headers = curl_slist_append(headers, api_key.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

        CURLcode res = curl_easy_perform(curl);
        bool success = (res == CURLE_OK);

        if (!success) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return success;
    }

    std::string url_;
    std::string key_;
};

} // namespace telemetry
} // namespace guardian
