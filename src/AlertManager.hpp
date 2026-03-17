#pragma once

#include "telemetry/TelegramClient.hpp"
#include "telemetry/AlertFormatter.hpp"
#include "telemetry/OfflineQueue.hpp"
#include <memory>
#include <iostream>

namespace guardian {

class AlertManager {
public:
    AlertManager(std::unique_ptr<telemetry::TelegramClient> client,
                 std::unique_ptr<telemetry::OfflineQueue> queue)
        : client_(std::move(client)), queue_(std::move(queue)) {}

    void on_detection(const telemetry::DetectionEvent& event) {
        if (event.confidence < 0.85f) {
            return;
        }

        std::string message = telemetry::AlertFormatter::format_detection(event);
        
        auto result = client_->send_message(message);
        if (!result) {
            std::cerr << "Falha ao enviar mensagem para o Telegram. Armazenando em fila offline." << std::endl;
            queue_->push(message);
        } else {
            // Tenta enviar mensagens pendentes
            auto pending = queue_->pop_all();
            for (const auto& msg : pending) {
                client_->send_message(msg);
            }
        }
    }

private:
    std::unique_ptr<telemetry::TelegramClient> client_;
    std::unique_ptr<telemetry::OfflineQueue> queue_;
};

} // namespace guardian
