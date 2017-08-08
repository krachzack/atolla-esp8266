#ifndef SHIFTREG_H
#define SHIFTREG_H

#include <bitset>
#include "config.h"

typedef std::bitset<SHIFTREG_OUTPUT_COUNT> ShiftregState;

/**
 * Configures pinmodes necessary for shiftreg to work
 */
void shiftreg_init();

void shiftreg_set(ShiftregState state);

#endif