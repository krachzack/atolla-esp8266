#ifndef CONFIG_H
#define CONFIG_H

// brown
#define PIN_GPIO_0 0
// yellow
#define PIN_GPIO_2 2
// white
#define PIN_TX 1
// green
#define PIN_RX 3

#define PIN_SHIFTREG_DATA PIN_RX
#define PIN_SHIFTREG_CLOCK_DATA PIN_GPIO_2
#define PIN_SHIFTREG_CLOCK_LATCH PIN_GPIO_0

#define SHIFTREG_OUTPUT_COUNT 6
// undef to use positive logic instead, i.e. shiftreg should  output HIGH for true
#define SHIFTREG_NEGATIVE_LOGIC

//#define CANVAS_USE_TIMER0
#define CANVAS_USE_TIMER1

#endif