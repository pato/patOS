#ifndef _PIT_H_
#define _PIT_H_

#include "stdint.h"

/* interrupt frequency = 1.193182 MHz / divide */
/* divide is a 16 bit unsigned value */
class Pit {
public:
    static uint32_t jiffies;
    static uint32_t hz;
    static uint32_t seconds();
    static void init(uint32_t hz);
    static void handler();
};

#endif
