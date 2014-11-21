#include "gdt.h"

#if 0
#include "mmu.h"
#include "stdint.h"

Descriptor gdt[GDT_COUNT] = {
    // index #0
    { 0, 0 },
    // index #1, selector 0x08, 32 bit code segment, DPL = 0
    { 0x0000FFFF,0x00CF9a00 },
    // index #2, selector 0x10, 32 bit data segment, DPL = 0
    { 0x0000FFFF,0x00CF9200 },
};

uint32_t kernelCS = GDT_KERNEL_CODE << 3;
uint32_t kernelDS = GDT_KERNEL_DATA << 3;

TableDescriptor gdtDesc = {
    (uint16_t) (sizeof(gdt) - 1),
    (uint32_t) gdt
};

void initGdt(void) {
}
#endif
