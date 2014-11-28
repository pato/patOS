#include "u8250pp.h"
#include "machine.h"
#include "process.h"
#include "vga.h"
#include "windowmanager.h"

/**
 * 8250++ - NextGen Driver
 * Uses vga for output
 * Uses console for input 
 */

U8250pp *U8250pp::it = nullptr;

void U8250pp::put(char c) {
  if (Process::current) {
    if  (c == '\n') {
      //TODO: child processes cannot print things because they dont have their own window!
      Window* currWindow = WindowManager::wm->windowMap.get(Process::current->id);
      if (currWindow) {
        currWindow->writeLine(false);
      }
    } else if (c == 13) {
      // ignore these chars
    } else {
      Window* currWindow = WindowManager::wm->windowMap.get(Process::current->id);
      if (currWindow) {
        currWindow->write(c);
      }
    }
  } else {
    while (!(inb(0x3F8+5) & 0x20));
    outb(0x3F8,c);
  }
}

char U8250pp::get() {
  Debug::panic("should not be called");
    getMutex.lock();
    while (!(inb(0x3F8+5) & 0x01)) {
       Process::yield();
    }
    char x = inb(0x3F8);
    getMutex.unlock();
    return x;
}
