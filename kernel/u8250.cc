#include "u8250.h"
#include "machine.h"
#include "process.h"

/* 8250 */

U8250 *U8250::it = nullptr;

void U8250::put(char c) {
    while (!(inb(0x3F8+5) & 0x20));
    outb(0x3F8,c);
}

char U8250::get() {
    getMutex.lock();
    while (!(inb(0x3F8+5) & 0x01)) {
       Process::yield();
    }
    char x = inb(0x3F8);
    getMutex.unlock();
    return x;
}
