#include "debug.h"
#include "machine.h"
#include "u8250.h"
#include "u8250pp.h"
#include "heap.h"
#include "init.h"
#include "pic.h"
#include "pit.h"
#include "tss.h"
#include "kbd.h"
#include "vmm.h"
#include "syscall.h"
#include "fs.h"
#include "ide.h"
#include "idle.h"
#include "window.h"
#include "vga.h"
#include "process.h"

Window* Process::window;


extern "C"
void kernelMain(void) {
    /* Here is the state:

          - we're running on the initial stack
          - there is no heap
          - interrupts are disabled
          - there are no processes

       Here is the plan:

          - direct debug output to COM1 so we can express ourselves
          - initialize the heap before someone starts saying new or malloc
          - initialize the interrupts controller and vectors
          - start the interval-timer ticking
          - create the first process and yield to it. This will implicitly
            enable interrupts
          - go away
    */
       
      
    Pic::off();             // make sure interrupts are disabled

    U8250 uart;
    U8250pp uartp;

    /* initialize serial console */
    U8250::init(&uart);
    U8250pp::init(&uartp);

    /* redirect debug output to COM1 */
    //Debug::init(U8250::it);
    Debug::init(U8250pp::it);
    Debug::debugAll = false;
    Debug::printf("\nWhat just happened? Who am I? Why am I here?\n");
    Debug::printf("I am K439, welcome to my world\n");

    /* initialize the TSS */
    TSS::init();

    /* Initialize system calls */
    Syscall::init();

    /* Initialize the heap */
    Heap::init((void*)0x100000,0x100000);
    Debug::printf("I have a heap\n");

    /* Initialize video drivers */
    VGA vga;

    Window(vga, "PatOS - version 0.0.0.0.1", 0, 0, 25, 80, VGA::GREEN, VGA::WHITE);

    int width = (80 - 3) / 2;
    int height = 25 - 3;

    //Process::window = new Window(vga, (const char*)"shell", 5, 5, 20, 70, VGA::BLUE, VGA::WHITE);
    Process::window = new Window(vga, (const char*)"shell", 2, 1, height, width, VGA::BLUE, VGA::WHITE);
    Window welcome = Window(vga, (const char*)"welcome", 2, 2 + width, height, width, VGA::LIGHT_RED, VGA::WHITE);

    /* Make the rest of memory available for VM */
    PhysMem::init(0x200000,0x400000);

    /* Initialize the process subsystem */
    Process::init();
    Process::DEBUG->off();
    Process::trace("Process tracing enabled");

    Pic::init();                // initialize the PIC, still disabled

    Keyboard::init();           // initialize the keyboard

    Pit::init(1000 /* Hz */);   // enable the PIT, interrupts still disabled

    /* hdd */
    IDE hdd(3);
    Process::trace("loaded driver for hdd");

    /* rootfs */
    FileSystem::init(new Fat439(&hdd));
    Process::trace("initialized root filesystem");

    /* Create the Primordial process */
    Process* initProcess = new Init();

    /* Create the idle process */
    Process::idleProcess = new IdleProcess();
    Process::idleProcess->start();

    initProcess->start();

    /* Yield to it, it will start running with interrupts enabled */
    Process::trace("Let there be processes");
    Process::yield();

    Debug::panic("The impossible has happened");
}
