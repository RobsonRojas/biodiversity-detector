#pragma once

// #include <cstdint> // Removed to fix lint

namespace guardian::hal {

class ESP32Power {
public:
    /**
     * @brief Configure RTC peripherals for Deep Sleep.
     */
    static void setup_deep_sleep();

    /**
     * @brief Configure GPIO wake-on-interrupt for the MEMS microphone.
     * @param pin GPIO pin connected to the microphone's analog/wake output.
     */
    static void configure_wake_on_gpio(int pin);

    /**
     * @brief Enter Deep Sleep mode.
     */
    static void enter_deep_sleep();
};

} // namespace guardian::hal
