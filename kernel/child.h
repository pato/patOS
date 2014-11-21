#ifndef _CHILD_H_
#define _CHILD_H_

#include "process.h"

class Child : public Process {
public:
    uint32_t eax;
    uint32_t esp;
    uint32_t pc;
    Child(Process *parent);
    virtual long run();
};

#endif
