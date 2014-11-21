#ifndef _VMM_H_
#define _VMM_H_

#include "stdint.h"

// The physical memory interface
class PhysMem {
    static uint32_t avail;
    
    struct Node {
        Node* next;
    };
    static Node *firstFree;
public:
    static constexpr uint32_t FRAME_SIZE = (1 << 12);
    static uint32_t limit;
    static void init(uint32_t start, uint32_t end);

    /* allocate a frame */
    static uint32_t alloc();

    /* free a frame */
    static void free(uint32_t);
};

class AddressSpace {
    uint32_t *pd;
private:
    uint32_t& getPTE(uint32_t va);
public:
    static constexpr uint32_t P = 1;
    static constexpr uint32_t W = 2;
    static constexpr uint32_t U = 4;

    AddressSpace();
    virtual ~AddressSpace();
    void punmap(uint32_t va);
    void pmap(uint32_t va, uint32_t pa, bool forUser, bool forWrite);
    void activate();
    void handlePageFault(long *context, uint32_t va);
    void dump();
    void fork(AddressSpace *child);
    void exec(); /* prepare for exec */
};

#endif
