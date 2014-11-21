#include "vmm.h"
#include "machine.h"
#include "process.h"
#include "mmu.h"
#include "idt.h"
#include "gdt.h"
#include "libk.h"
#include "err.h"

PhysMem::Node *PhysMem::firstFree = 0;
uint32_t PhysMem::avail;
uint32_t PhysMem::limit;

void PhysMem::init(uint32_t start, uint32_t end) {
    avail = start;
    limit = end;
    firstFree = 0;

    /* register the page fault handler */
    setTrapDescriptor(&idt[14],kernelCodeSeg,(uint32_t)pageFaultHandler,0);
}

uint32_t PhysMem::alloc() {
    Process::disable();
    uint32_t p;

    if (firstFree) {
        p = (uint32_t) firstFree;
        firstFree = firstFree->next;
    } else {
        if (avail == limit) {
            Debug::panic("no more frames");
        }
        p = avail;
        avail += FRAME_SIZE;
    }
    Process::enable();

    K::bzero((void*)p,FRAME_SIZE);

    return p;
}

void PhysMem::free(uint32_t p) {
    Process::disable();

    Node* n = (Node*) p;    
    n->next = firstFree;
    firstFree = n;

    Process::enable();
}


AddressSpace::AddressSpace() {
    pd = (uint32_t*) PhysMem::alloc();
    for (uint32_t va = PhysMem::FRAME_SIZE;
        va < PhysMem::limit;
        va += PhysMem::FRAME_SIZE
    ) {
        pmap(va,va,false,true);
    }
    //dump();
}

AddressSpace::~AddressSpace() {
    for (int i0 = 0; i0 < 1024; i0++) {
        uint32_t pde = pd[i0];
        if (pde & P) {
            uint32_t *pt = (uint32_t*) (pde & 0xfffff000);
            if (i0 > 0) {
                for (uint32_t i1 = 0; i1 < 1024; i1++) {
                    uint32_t pte = pt[i1];
                    if (pte & P) {
                        uint32_t pa = pte & 0xfffff000;
                        PhysMem::free(pa);
                    }
                }
            }
            PhysMem::free((uint32_t) pt);
        }
    }
    PhysMem::free((uint32_t) pd);
}
    
void AddressSpace::dump() {
    for (int i0 = 0; i0 < 1024; i0++) {
        uint32_t pde = pd[i0];
        if (pde & P) {
            Debug::printf("%d\n",i0);
            uint32_t *pt = (uint32_t*) (pde & 0xfffff000);
            uint32_t high = i0 << 22;
            for (uint32_t i1 = 0; i1 < 1024; i1++) {
                uint32_t pte = pt[i1];
                if (pte & P) {
                    Debug::printf("    %x -> %x\n",high | (i1 << 12),pte);
                }
            }
        }
    }
}

/* precondition: table is locked */
uint32_t& AddressSpace::getPTE(uint32_t va) {
    uint32_t i0 = (va >> 22) & 0x3ff;
    if ((pd[i0] & P) == 0) {
        pd[i0] = PhysMem::alloc() | 7; /* UWP */
    }
    uint32_t* pt = (uint32_t*) (pd[i0] & 0xfffff000);
    return pt[(va >> 12) & 0x3ff];
}

void AddressSpace::punmap(uint32_t va) {
    Process::disable();
    getPTE(va) = 0;
    invlpg(va);
    Process::enable();
}
        
void AddressSpace::pmap(uint32_t va, uint32_t pa, bool forUser, bool forWrite) {
    Process::disable();
    invlpg(va);
    getPTE(va) = (pa & 0xfffff000) |
          (forUser ? U : 0) | (forWrite ? W : 0) | 1;
    Process::enable();
}

void AddressSpace::activate() {
    Process::disable();
    vmm_on((uint32_t)pd);
    Process::enable();
}

void AddressSpace::handlePageFault(long* context, uint32_t va) {
    //Process::trace("page fault @ %x",va);
    if (va < 0x1000) {
        Debug::printf("process %s, page fault %x\n",Process::current->name,va);
        Process::current->kill(ERR_PAGE_FAULT);
    } else {
        if (va >= 0x80000000) {
            pmap(va,PhysMem::alloc(),true,true);
        } else {
            Debug::panic("page fault @ %x",va);
        }
    }
}

void AddressSpace::fork(AddressSpace* child) {
    for (int i0 = 512; i0 < 1024; i0++) {
        uint32_t pde = pd[i0];
        if (pde & P) {
            uint32_t *pt = (uint32_t*) (pde & 0xfffff000);
            uint32_t high = i0 << 22;
            for (uint32_t i1 = 0; i1 < 1024; i1++) {
                uint32_t pte = pt[i1];
                if (pte & P) {
                    uint32_t va = high | (i1 << 12);
                    uint32_t src = pte & ~0xfff;
                    uint32_t dest = PhysMem::alloc();
                    memcpy((void*)dest,(void*)src,PhysMem::FRAME_SIZE);
                    child->pmap(va,dest,true,true); 
                }
            }
        }
    }
}

void AddressSpace::exec() {
    for (int i0 = 512; i0 < 1024; i0++) {
        uint32_t pde = pd[i0];
        if (pde & P) {
            uint32_t *pt = (uint32_t*) (pde & 0xfffff000);
            uint32_t high = i0 << 22;
            for (uint32_t i1 = 0; i1 < 1024; i1++) {
                uint32_t pte = pt[i1];
                if (pte & P) {
                    uint32_t va = high | (i1 << 12);
                    uint32_t pa = pte & ~0xfff;
                    pt[i1] = 0;
                    invlpg(va);
                    PhysMem::free(pa);
                }
            }
        }
    }
}

extern "C" void vmm_pageFault(long* context, uintptr_t va) {
    Process* proc = Process::current;
    if (!proc) {
        for (int i=0; i<20; i++) {
            Debug::printf("%d -> %x\n",i,context[i]);
        }
        Debug::panic("page fault @ 0x%08x without current process",va);
    }
    proc->addressSpace.handlePageFault(context,va);
}
