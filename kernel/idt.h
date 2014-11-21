#ifndef _IDT_H_
#define _IDT_H_

#include "stdint.h"
#include "mmu.h"

#define IDT_COUNT 256

extern Descriptor idt[];

//extern TableDescriptor idtDesc;

class IDT {
public:
    static void init(void);
    static void addInterruptHandler(int index, uint32_t handler);
    static void addTrapHandler(int index, uint32_t handler, int dpl);
};

#endif
