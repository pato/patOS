#ifndef _KBD_H_
#define _KBD_H_

#include "io.h"

class Keyboard {
    
public:
    static InputStream<char> *is;
    static void handler(void);
    static void init(void);
};

#endif
