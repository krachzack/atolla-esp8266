#include <Arduino.h>
#include "shiftreg.h"

void shiftreg_init() {
    pinMode(PIN_SHIFTREG_DATA, OUTPUT);
    pinMode(PIN_SHIFTREG_CLOCK_DATA, OUTPUT);
    pinMode(PIN_SHIFTREG_CLOCK_LATCH, OUTPUT);
}

void shiftreg_set(ShiftregState state) {
    digitalWrite(PIN_SHIFTREG_CLOCK_DATA, LOW);
    digitalWrite(PIN_SHIFTREG_CLOCK_LATCH, LOW);

    for(int i = state.size(); i >= 0; --i) {
        #ifdef SHIFTREG_NEGATIVE_LOGIC
        int val = state[i] ? LOW : HIGH;
        #else
        int val = state[i] ? HIGH : LOW;
        #endif

        digitalWrite(PIN_SHIFTREG_DATA, val);
        digitalWrite(PIN_SHIFTREG_CLOCK_DATA, HIGH);
        digitalWrite(PIN_SHIFTREG_CLOCK_DATA, LOW);
    }

    digitalWrite(PIN_SHIFTREG_CLOCK_LATCH, HIGH);
}
