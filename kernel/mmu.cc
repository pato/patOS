#include "mmu.h"

#include "stdint.h"
#include "machine.h"

/**********************/
/* Descriptor helpers */
/**********************/

// generate a mask of 1's between the given bit positions
static inline uint32_t mask(uint32_t leftBit, uint32_t rightBit) {
    leftBit &= 0x1f;
    rightBit &= 0x1f;
    uint32_t n = leftBit - rightBit + 1;
    return ((((uint32_t) 1) << n) - 1) << rightBit;
}

// merge two quantities after shifting and masking
static inline void mms(uint32_t *p, uint32_t v, uint32_t leftBit, uint32_t rightBit) {
    uint32_t sm = mask(leftBit,rightBit);
    *p =  (*p & ~sm) | ((v << rightBit) & sm);
}

// extract by masking and shifting
static inline uint32_t mme(uint32_t v, uint32_t leftBit, uint32_t rightBit) {
    return (v & mask(leftBit,rightBit)) >> rightBit;
}

void setNullDescriptor(Descriptor* p) {
    p->f0 = 0;
    p->f1 = 0;
}

void setP(Descriptor* p, uint32_t v) {
    mms(&p->f1,v,15,15);
}

uint32_t getP(Descriptor* p) {
    return mme(p->f1,15,15);
}

void setG(Descriptor* p, uint32_t v) {
    mms(&p->f1,v,23,23);
}

uint32_t getG(Descriptor* p) {
    return mme(p->f1,23,23);
}

void setD(Descriptor* p, uint32_t d) {
    mms(&p->f1,d,22,22);
}

uint32_t getD(Descriptor* p) {
    return mme(p->f1,22,22);
}

void setB(Descriptor* p, uint32_t d) {
    setD(p,d);
}

uint32_t getB(Descriptor* p) {
    return getD(p);
}

void setSelector(Descriptor *p, uint32_t selector) {
    mms(&p->f0, selector, 31, 16);
}

void setOffset(Descriptor *p, uint32_t offset) {
    mms(&p->f0,       offset, 15,  0);
    mms(&p->f1, offset >> 16, 31, 16);
}

void setBase(Descriptor* p, uint32_t base) {
    mms(&p->f0,base >>  0,31,16);
    mms(&p->f1,base >> 16, 7, 0);
    mms(&p->f1,base >> 24,31,24);
}

uint32_t getBase(Descriptor *p) {
    return mme(p->f0,31,16) | (mme(p->f1,7,0) << 16) | (mme(p->f1,31,24) << 24);
}

void setLimit(Descriptor* p, uint32_t limit) {
    mms(&p->f0,limit      , 15,  0);
    mms(&p->f1,limit >> 16, 19, 16);
}

uint32_t getLimit(Descriptor* p) {
    return mme(p->f0,15,0) | (mme(p->f1,19,16) << 16);
}

void setDPL(Descriptor* p, uint32_t dpl) {
    mms(&p->f1, dpl, 14, 13);
}

uint32_t getDPL(Descriptor* p) {
    return mme(p->f1, 14, 13);
}

void setS(Descriptor* p, uint32_t s) {
    mms(&p->f1, s, 12, 12);
}

uint32_t getS(Descriptor* p) {
    return mme(p->f1, 12, 12);
}

void setType(Descriptor* p, uint32_t t) {
    mms(&p->f1, t, 11, 8);
}

uint32_t getType(Descriptor* p) {
    return mme(p->f1, 11, 8);
}

// kind : 1 -> code, 0 -> data
void setK(Descriptor* p, uint32_t v) {
    mms(&p->f1, v, 11, 11);
}

uint32_t getK(Descriptor* p) {
    return mme(p->f1, 11, 11);
}

void setC(Descriptor* p, uint32_t v) {
    mms(&p->f1, v, 10, 10);
}

uint32_t getC(Descriptor* p) {
    return mme(p->f1, 10, 10);
}

void setR(Descriptor* p, uint32_t v) {
    mms(&p->f1, v, 9, 9);
}

uint32_t getR(Descriptor* p) {
    return mme(p->f1, 9, 9);
}

void setA(Descriptor* p, uint32_t v) {
    mms(&p->f1, v, 8, 8);
}

uint32_t getA(Descriptor* p) {
    return mme(p->f1, 8, 8);
}

void setE(Descriptor* p, uint32_t v) {
    mms(&p->f1, v, 10, 10);
}

uint32_t getE(Descriptor* p) {
    return mme(p->f1, 10, 10);
}

void setW(Descriptor* p, uint32_t v) {
    mms(&p->f1, v, 9, 9);
}

uint32_t getW(Descriptor* p) {
    return mme(p->f1, 9, 9);
}

void setCodeDescriptor(
    Descriptor* p,
    uint32_t base,
    uint32_t limit,
    uint32_t dpl)
{
    setNullDescriptor(p);
    setBase(p,base);
    if (limit > 0xfffff) {
        setLimit(p,limit >> 12);
        setG(p,1);
    } else {
        setLimit(p,limit);
        setG(p,0);
    }
    setDPL(p,dpl);
    setP(p,1);
    setD(p,1);
    setS(p,1);
    setK(p,1);
    setC(p,1);
    setR(p,1);
    setA(p,0);
}

void setDataDescriptor(
    Descriptor* p,
    uint32_t base,
    uint32_t limit,
    uint32_t dpl)
{
    setNullDescriptor(p);
    setBase(p,base);
    if (limit > 0xfffff) {
        setG(p,1);
        setLimit(p,limit >> 12);
    } else {
        setG(p,0);
        setLimit(p,limit);
    }
    setDPL(p,dpl);
    setP(p,1);
    setB(p,1);
    setS(p,1);
    setK(p,0);
    setW(p,1);
    setE(p,0);
    setA(p,0);
}

void setTssDescriptor(Descriptor *p, uint32_t base, uint32_t limit) {
    setNullDescriptor(p);
    setBase(p,base);
    setLimit(p,limit);
    setP(p,1);
    setDPL(p,0);
    setS(p,0);
    setType(p,0x9);
}

void setTrapDescriptor(Descriptor *p, uint32_t selector, uint32_t offset,
    uint32_t dpl)
{
    setNullDescriptor(p);
    setSelector(p,selector);
    setOffset(p,offset);
    setP(p,1);
    setDPL(p,dpl);
    setS(p,0);
    setType(p, 0xF);
}

void setInterruptDescriptor(Descriptor *p, uint32_t selector, uint32_t offset,
    uint32_t dpl)
{
    setNullDescriptor(p);
    setSelector(p,selector);
    setOffset(p,offset);
    setP(p,1);
    setDPL(p,dpl);
    setS(p,0);
    setType(p, 0xE);
}
