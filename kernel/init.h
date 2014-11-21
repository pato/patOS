#ifndef _INIT_H_
#define _INIT_H_

#include "process.h"

class Init : public Process {
public:
    Init();
    virtual long run();
};

#endif
