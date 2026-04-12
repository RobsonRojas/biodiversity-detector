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

#include "i2s_low_level.hpp"
#include "esp_log.h"
#include "esp_check.h"
#include <string.h>

static const char* TAG = "I2S_LL_STABLE";

I2SLowLevel::I2SLowLevel() : rx_handle(nullptr), rb(nullptr), initialized(false) {}

I2SLowLevel::~I2SLowLevel() = default;

esp_err_t I2SLowLevel::init(const i2s_ll_config_t& config, SharedRingBuffer* ring_buf) {
    cfg = config;
    rb = ring_buf;
    
    if (!rb) return ESP_ERR_INVALID_ARG;

    // 1. Channel Configuration
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_chan_handle_t raw_rx_handle = nullptr;
    ESP_RETURN_ON_ERROR(i2s_new_channel(&chan_cfg, NULL, &raw_rx_handle), TAG, "Failed to create I2S channel");
    
    // Transfer ownership to unique_ptr
    rx_handle.reset(raw_rx_handle);

    // 2. Standard Mode Configuration
    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(cfg.sample_rate),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = cfg.bck_io,
            .ws   = cfg.ws_io,
            .dout = I2S_GPIO_UNUSED,
            .din  = cfg.din_io,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    // Ensure 32-bit slot for high fidelity
    std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT; 
    
    ESP_RETURN_ON_ERROR(i2s_channel_init_std_mode(rx_handle.get(), &std_cfg), TAG, "Failed to init I2S std mode");

    // 3. Register Event Callback
    i2s_event_callbacks_t cbs = {
        .on_recv = i2s_rx_callback,
        .on_recv_q_ovf = NULL,
        .on_sent = NULL,
        .on_send_q_ovf = NULL,
    };
    ESP_RETURN_ON_ERROR(i2s_channel_register_event_callback(rx_handle.get(), &cbs, this), TAG, "Failed to register callback");

    initialized = true;
    ESP_LOGI(TAG, "I2S Stable Driver initialized (32-bit, %lu Hz)", cfg.sample_rate);
    return ESP_OK;
}

esp_err_t I2SLowLevel::start() {
    if (!initialized) return ESP_ERR_INVALID_STATE;
    return i2s_channel_enable(rx_handle.get());
}

esp_err_t I2SLowLevel::stop() {
    if (!initialized) return ESP_ERR_INVALID_STATE;
    return i2s_channel_disable(rx_handle.get());
}

bool IRAM_ATTR I2SLowLevel::i2s_rx_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx) {
    I2SLowLevel* self = (I2SLowLevel*)user_ctx;
    
    // In standard driver, event->data contains the received buffer
    // and event->size is the length.
    // We copy it to the SharedRingBuffer.
    
    void* ring_ptr = self->rb->get_free_segment_ptr();
    if (ring_ptr) {
        memcpy(ring_ptr, event->dma_buf, event->size);
        self->rb->commit_segment();
    }
    
    return false; // No higher priority task woken
}
