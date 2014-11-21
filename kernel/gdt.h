#ifndef _GDT_H_
#define _GDT_H_


#include "stdint.h"
#include "mmu.h"

extern uint32_t kernelCodeSeg;
extern uint32_t kernelDataSeg;
extern uint32_t userCodeSeg;
extern uint32_t userDataSeg;

extern uint32_t tssDS;
extern Descriptor tssDescriptor;


#endif
