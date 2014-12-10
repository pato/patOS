#ifndef _KBD_H_
#define _KBD_H_

#include "io.h"
#include "bb.h"

class Keyboard {
    
public:
    static InputStream<char> *is;
    static BB<char> *bb;
    static void handler(void);
    static void init(void);
};

#endif
