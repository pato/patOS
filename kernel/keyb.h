#ifndef _U8250_H_
#define _U8250_H_

/* 8250 */

#include "io.h"
#include "semaphore.h"

class U8250 : public OutputStream<char> {
    Mutex getMutex;
public:
    static U8250 *it;
    static void init(U8250 *p) {
        it = p;
    }

    U8250() {}
    virtual void put(char ch);
    virtual char get();
};

#endif
