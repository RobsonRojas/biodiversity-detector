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
            {"sound_class", sound_class},
            {"pushed_at", "now()"} // Let Postgrest handle timestamp or use now()
        };
        if (!audio_url.empty()) {
            payload["audio_url"] = audio_url;
        }
        return send_post(url_ + "/rest/v1/detections", payload.dump());
    }

    bool post_heartbeat(const std::string& node_id, int battery, int rssi) {
        nlohmann::json payload = {
            {"id", node_id},
            {"status", "online"},
            {"battery_mv", battery},
            {"rssi_dbm", rssi},
            {"last_seen", "now()"}
        };
        // Upsert nodes table
        return send_post(url_ + "/rest/v1/nodes?select=id&on_conflict=id", payload.dump(), "POST", true);
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
    bool send_post(const std::string& full_url, const std::string& data, const std::string& method = "POST", bool upsert = false) {
        CURL* curl = curl_easy_init();
        if (!curl) return false;

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string auth = "Authorization: Bearer " + key_;
        headers = curl_slist_append(headers, auth.c_str());
        std::string api_key = "apikey: " + key_;
        headers = curl_slist_append(headers, api_key.c_str());
        
        if (upsert) {
            headers = curl_slist_append(headers, "Prefer: resolution=merge-duplicates");
        }

        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        if (method == "PATCH") {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        }

        CURLcode res = curl_easy_perform(curl);
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        
        bool success = (res == CURLE_OK && response_code >= 200 && response_code < 300);

        if (!success) {
            std::cerr << "[Supabase] Request failed (" << response_code << "): " << curl_easy_strerror(res) << " to " << full_url << std::endl;
            std::cerr << "[Supabase] Payload: " << data << std::endl;
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
