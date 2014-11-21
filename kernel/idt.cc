#include "idt.h"
#include "gdt.h"
#include "machine.h"

void IDT::init(void) {
}

void IDT::addTrapHandler(int index, uint32_t handler, int dpl) {
    setTrapDescriptor(&idt[index],kernelCodeSeg, handler, dpl);
}

void IDT::addInterruptHandler(int index, uint32_t handler) {
    setInterruptDescriptor(&idt[index],kernelCodeSeg, handler, 0);
}
