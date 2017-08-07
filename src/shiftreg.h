#ifndef SHIFTREG_H
#define SHIFTREG_H

#include <bitset>
#include "config.h"

typedef std::bitset<SHIFTREG_OUTPUT_COUNT> ShiftregState;

void shiftreg_set(ShiftregState state);

#endif