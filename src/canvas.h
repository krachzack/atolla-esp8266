#ifndef CANVAS_H
#define CANVAS_H

#include "config.h"
#include "shiftreg.h"
#include <vector>
#include <array>
#include <cstdint>

class Canvas {
public:
    static Canvas instance;

    void clear() { thresholds.fill(0); }

    // Set the colors to the given interleaved rgb colors
    // If canvas has more outputs than colors are provided,
    // the colors are repeated
    // If more colors given, additional colors are ignored
    void paint(const std::vector<uint8_t>& colors);

private:
    Canvas() { init_update_timer(); }

    void init_update_timer();
    friend void ICACHE_RAM_ATTR handle_timer_interrupt();

    // For each output holds the remaining iterations for the
    std::array<uint8_t, SHIFTREG_OUTPUT_COUNT> thresholds;
};

#endif