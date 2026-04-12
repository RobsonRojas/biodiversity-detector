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

#include "esp_err.h"
#include "driver/i2s_std.h"
#include <memory>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "shared_ring_buffer.hpp"

typedef struct {
    uint32_t sample_rate;
    gpio_num_t bck_io;
    gpio_num_t ws_io;
    gpio_num_t din_io;
} i2s_ll_config_t;

class I2SLowLevel {
public:
    I2SLowLevel();
    ~I2SLowLevel();

    esp_err_t init(const i2s_ll_config_t& config, SharedRingBuffer* ring_buf);
    esp_err_t start();
    esp_err_t stop();

private:
    struct I2SDeleter {
        void operator()(i2s_chan_handle_t h) const {
            if (h) {
                i2s_channel_disable(h);
                i2s_del_channel(h);
            }
        }
    };

    std::unique_ptr<std::remove_pointer_t<i2s_chan_handle_t>, I2SDeleter> rx_handle;
    i2s_ll_config_t cfg;
    SharedRingBuffer* rb;
    bool initialized;

    // Use callback for DMA data handling
    static bool IRAM_ATTR i2s_rx_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx);
};
