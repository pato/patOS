#ifndef _MMU_H_
#define _MMU_H_

#include "stdint.h"

typedef struct __attribute__ ((__packed__)) TableDescriptor {
    uint16_t limit;
    uint32_t base;
} TableDescriptor;

struct Descriptor {
    uint32_t f0;
    uint32_t f1;
};

static inline uint32_t globalSelector(uint32_t index, uint32_t dpl) {
    return (index << 3) + (dpl & 0x3);
}

static inline uint32_t localSelector(uint32_t index, uint32_t dpl) {
    return (index << 3) + 2 + (dpl & 0x3);
}

extern void nullDescriptor(Descriptor* p);

extern void setL(Descriptor* p, uint32_t v);
extern uint32_t getL(Descriptor* p);

extern void setP(Descriptor* p, uint32_t v);
extern uint32_t getP(Descriptor* p);

extern void setG(Descriptor* p, uint32_t v);
extern uint32_t getG(Descriptor* p);

extern void setBase(Descriptor* p, uint32_t base);
extern uint32_t getBase(Descriptor *p);

extern void setLimit(Descriptor* p, uint32_t limit);
extern uint32_t getLimit(Descriptor* p);

extern void setDpl(Descriptor* p, int v);
extern uint32_t getDpl(Descriptor* p);

extern void setCodeDescriptor( Descriptor* p, uint32_t base, uint32_t limit,
    uint32_t dpl);

extern void setDataDescriptor( Descriptor* p, uint32_t base, uint32_t limit,
    uint32_t dpl);

extern void setTssDescriptor(Descriptor* p, uint32_t base, uint32_t limit);

extern void setTrapDescriptor( Descriptor* p, uint32_t selector,
    uint32_t offset, uint32_t dpl);

extern void setInterruptDescriptor( Descriptor* p, uint32_t selector,
    uint32_t offset, uint32_t dpl);

#endif
