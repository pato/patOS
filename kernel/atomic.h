#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include "stdint.h"

class Atomic32 {
    volatile uint32_t v;
public:
    Atomic32(uint32_t v = 0) : v(v) {}
    inline uint32_t getThenAdd(uint32_t d) {
        uint32_t out;
        asm volatile ("lock xadd %[out],%[v]"
            : [out] "=r" (out), [v] "=m"(v)
            : "0"(d), "m"(v)
            : "memory", "cc"
        );
        return out;
    }
    inline uint32_t get() {
        return v;
    }
    inline void set(uint32_t x) {
        v = x;
    }
};


#endif
