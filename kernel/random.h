#ifndef _RANDOM_H_
#define _RANDOM_H_

#include "stdint.h"


// multiply-with-carry pseudo-random number generator by George Marsaglia
// source: http://en.wikipedia.org/wiki/Random_number_generation

class Random32 {
    uint32_t w;
    uint32_t z;
public:
    Random32(uint32_t seed) {
        w = seed;
        z = ~seed;
        if ((w == 0) || (w == 0x464fffff)) {
            w += 1;
        }
        if ((z == 0) || (z == 0x9068ffff)) {
            z += 1;
        }
    }
    uint32_t next() {
        z = 36969 * (z & 65535) + (z >> 16);
        w = 18000 * (w & 65535) + (w >> 16);
        return (z << 16) + w;
    }
};

#endif
