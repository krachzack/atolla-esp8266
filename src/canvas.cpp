#include <Arduino.h>
#include "canvas.h"
#include "shiftreg.h"

Canvas Canvas::instance;

// in us
#define REFRESH_INTERVAL 18

#define TIMER1_TICKS_PER_US (APB_CLK_FREQ / 1000000L)
uint32_t usToTicks(uint32_t us)
{
    return (TIMER1_TICKS_PER_US / 1 * us);     // converts microseconds to tick
}

void ICACHE_RAM_ATTR handle_timer_interrupt() {
    noInterrupts();

    static ShiftregState last_shiftreg_state;
    // automatically wraps at 255
    static uint8_t pwm_iteration = 0;

    ShiftregState state;

    Canvas& c = Canvas::instance;

    size_t state_idx = 0;
    for(uint8_t threshold : c.thresholds) {
        // no need for range check, size of thresholds is same size as shiftregstate
        state[state_idx++] = threshold > pwm_iteration;
    }

    if(state != last_shiftreg_state) {
        shiftreg_set(state);
        last_shiftreg_state = state;
    }

    ++pwm_iteration;

    interrupts();
}

void Canvas::init_update_timer() {
    timer1_disable();
    timer1_isr_init();
    timer1_attachInterrupt(handle_timer_interrupt);
    // this is about as fast as it gets
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
    timer1_write(usToTicks(REFRESH_INTERVAL));
}

void Canvas::paint(const std::vector<uint8_t>& colors) {
    if(colors.size() == 0) {
        // If empty vector given, erase output
        clear();
    } else {
        // Otherwise fill the tresholds with the colors and repeat the pattern
        // if too short to fill thresholds
        size_t thresholds_idx = 0;

        while(thresholds_idx < thresholds.size()) {
            for(uint8_t color : colors) {
                // If thresholds are full before colors are exhausted, just return
                if(thresholds_idx >= thresholds.size()) {
                    return;
                }

                // directly translate color value to threshold, this maybe should be scaled
                thresholds[thresholds_idx] = color;

                ++thresholds_idx;
            }
        }
    }
}
