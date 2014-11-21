#ifndef _U8250pp_H_
#define _U8250pp_H_

/**
 * 8250++ - NextGen Driver
 * Uses vga for output
 * Uses console for input 
 */

#include "io.h"
#include "semaphore.h"

class U8250pp : public OutputStream<char> {
    Mutex getMutex;
public:
    static U8250pp *it;
    static void init(U8250pp *p) {
        it = p;
    }

    U8250pp() {}
    virtual void put(char ch);
    virtual char get();
};

#endif
