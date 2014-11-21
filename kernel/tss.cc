#include "tss.h"
#include "machine.h"
#include "gdt.h"
#include "mmu.h"

struct {
    uint32_t prev;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t unused[19];
} tss;

void TSS::init(void) {
    tss.esp0 = 0;
    tss.ss0 = kernelDataSeg;
    setTssDescriptor(&tssDescriptor,(uint32_t)&tss,sizeof(tss));
    ltr(tssDS);
}

void TSS::esp0(uint32_t v) {
    tss.esp0 = v;
}
