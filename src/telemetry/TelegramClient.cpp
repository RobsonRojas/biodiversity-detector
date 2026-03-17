#include "TelegramClient.hpp"
#include <curl/curl.h>
#include "utils/json.hpp"
#include <format>
#include <iostream>

namespace guardian::telemetry {

namespace {
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
}

TelegramClient::TelegramClient(TelegramConfig config) : config_(std::move(config)) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

std::expected<void, TelemetryError> TelegramClient::send_message(const std::string& message) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return std::unexpected(TelemetryError::Unknown);
    }

    std::string url = std::format("https://api.telegram.org/bot{}/sendMessage", config_.bot_token);
    
    nlohmann::json payload;
    payload["chat_id"] = config_.chat_id;
    payload["text"] = message;
    payload["parse_mode"] = "HTML";

    std::string json_str = payload.dump();
    std::string response_string;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    CURLcode res = curl_easy_perform(curl);
    
    long response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return std::unexpected(TelemetryError::NetworkError);
    }

    if (response_code != 200) {
        return std::unexpected(TelemetryError::ApiError);
    }

    return {};
}

} // namespace guardian::telemetry
