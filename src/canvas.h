
#ifndef CANVAS_H
#define CANVAS_H

#include <Arduino.h>
#include "config.h"
#include <bitset>
#include <vector>
#include <array>
#include <cstdint>

class Canvas {
public:
    static Canvas instance;

    void begin() { setup_shiftreg(); }

    void update();

    void setAutoUpdate() { init_watchdog(); init_update_timer(); }

    void clear() { thresholds.fill(0); }

    // Set the colors to the given interleaved rgb colors
    // If canvas has more outputs than colors are provided,
    // the colors are repeated
    // If more colors given, additional colors are ignored
    void paint(const std::vector<uint8_t>& colors);

    void paint(uint8_t r, uint8_t g, uint8_t b) {
        std::vector<uint8_t> pattern = {r, g, b};
        paint(pattern);
    }

private:
    void init_update_timer();
    void setup_shiftreg();
    void init_watchdog();

    friend void ICACHE_RAM_ATTR handle_timer_interrupt();

    // For each output holds the remaining iterations for the
    std::array<uint8_t, SHIFTREG_OUTPUT_COUNT> thresholds;
};

#endif
