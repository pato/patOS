#include "init.h"
#include "elf.h"
#include "machine.h"
#include "fs.h"
#include "libk.h"
#include "process.h"
#include "kbd.h"

Init::Init() : Process("init",nullptr) {
}

long Init::run() {
  /*
  SimpleQueue<const char*> argv;
  argv.addTail(K::strdup("welcome"));
  execv("welcome",&argv,1);
  */

  SimpleQueue<const char*> argv;
  argv.addTail(K::strdup("shell"));
  Keyboard::bb->put('w');
  Keyboard::bb->put('e');
  Keyboard::bb->put('l');
  Keyboard::bb->put('c');
  Keyboard::bb->put('o');
  Keyboard::bb->put('m');
  Keyboard::bb->put('e');
  Keyboard::bb->put(13);
  execv("shell",&argv,1);

  Debug::shutdown("What?");
  return 0;
}
