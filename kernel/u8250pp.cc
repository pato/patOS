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
    Window* currWindow = WindowManager::wm->currentWindow();
    if  (c == '\n') { /* new lines */
      if (currWindow) currWindow->writeLine(false);
    } else if (c == 13) { /* ignored characters */
    } else { /* normal characters */
      if (currWindow) currWindow->write(c);
    }
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
