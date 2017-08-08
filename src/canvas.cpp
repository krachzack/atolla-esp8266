#include <Arduino.h>
#include "canvas.h"
#include "shiftreg.h"

Canvas Canvas::instance;

#ifdef CANVAS_USE_TIMER1
    // in us
    #define REFRESH_INTERVAL 30

    #define TIMER1_TICKS_PER_US (APB_CLK_FREQ / 1000000L)
    uint32_t usToTicks(uint32_t us)
    {
        return (TIMER1_TICKS_PER_US / 1 * us);     // converts microseconds to tick
    }

#else

    #define TIMER1_CYCLES 2000
    volatile unsigned long next;

#endif

void ICACHE_RAM_ATTR handle_timer_interrupt() {
    //noInterrupts();

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

    pwm_iteration += 2;

    // Reset watchdog
    //wdt_reset();

    //interrupts();

#ifdef CANVAS_USE_TIMER0
    next=ESP.getCycleCount() + TIMER1_CYCLES;
    timer0_write(next);
#endif
}

void Canvas::update() {
    static uint32_t last_cycles = 0;
    uint32_t cycles = ESP.getCycleCount();

    if(cycles % 2048 < last_cycles % 2048) {
        handle_timer_interrupt();
    }

    last_cycles = cycles;
}

void Canvas::init_update_timer() {
    #ifdef CANVAS_USE_TIMER1

        timer1_disable();
        timer1_isr_init();
        timer1_attachInterrupt(handle_timer_interrupt);
        // this is about as fast as it gets
        timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
        timer1_write(usToTicks(REFRESH_INTERVAL));

    #else

        timer0_isr_init();
        timer0_attachInterrupt(handle_timer_interrupt);
        next=ESP.getCycleCount() + TIMER1_CYCLES;
        timer0_write(next);

    #endif
}

void Canvas::init_watchdog() {
    // Set up ESP watchdog, otherwise timer1 will cause resets
    //ESP.wdtDisable();
    //ESP.wdtEnable(WDTO_8S);
}

void Canvas::setup_shiftreg() {
    shiftreg_init();
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
