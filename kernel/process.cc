#include "process.h"
#include "machine.h"
#include "debug.h"
#include "pic.h"
#include "pit.h"
#include "semaphore.h"
#include "idle.h"
#include "tss.h"
#include "elf.h"
#include "fs.h"
#include "err.h"
#include "libk.h"
#include "windowmanager.h"

/* global process declarations */
Debug* Process::DEBUG;                       // the debug channel
size_t Process::STACK_LONGS = 1024 * 2;      // default kernel stack size
SimpleQueue<Process*> *Process::readyQueue;  // the ready queue
SimpleQueue<Process*> *Process::reaperQueue; // the reaper queue
Process* Process::current;                   // the current process
Atomic32 Process::nextId;                     // next process ID
Semaphore *Process::traceMutex;
Timer* Process::timers = nullptr;            // pending timers
Process* Process::idleProcess = nullptr;     // the idle process
uint32_t Process::idleJiffies = 0;            // idle jiffies

void Process::init() {
    DEBUG = new Debug("Process");
    readyQueue = new SimpleQueue<Process*>();
    reaperQueue = new SimpleQueue<Process*>();
    traceMutex = new Semaphore(1);
}

void Process::checkReaper() {
    Process *kill;
    do {
        kill = nullptr;
        Process::disable();
        if (!Process::reaperQueue->isEmpty()) {
            kill = Process::reaperQueue->removeHead();
        }
        Process::enable();
        Resource::unref(kill);
    } while (kill != nullptr);
}

void Process::checkKilled() {
    checkReaper();
    Process *me = Process::current;
    if (me) {
        if (me->isKilled) {
            me->onKill();
            Process::exit(me->killCode);
        }
    }
}

void Process::entry() {
    checkKilled();
    long code = run();
    exit(code);
}

extern "C" void runProcess() {
    Process::current->entry();
}

/* A bit of room to detect stack overflow without
   corrupting neighbors */
#define FUDGE 128

Process::Process(const char* name, Table *resources_) : 
    Resource(ResourceType::PROCESS), name(name), resources(resources_)
{
    //Debug::printf("Process::Process %p",this);
    id = nextId.getThenAdd(1);
    iDepth = 0;
    iCount = 0;
    isKilled = false;
    killCode = 0;
    hasWindow = false;
    disableCount = 0;
    stack = new long[FUDGE + STACK_LONGS];
    if (stack == 0) {
        Debug::panic("can't allocate stack");
    }
    stack = &stack[FUDGE];
    int idx = STACK_LONGS;
    stack[--idx] = 0;
    stack[--idx] = (long) runProcess;
    stack[--idx] = 0; /* %ebx */
    stack[--idx] = 0; /* %esi */
    stack[--idx] = 0; /* edi */
    stack[--idx] = 0; /* ebp */
    kesp = (long) &stack[idx];

    /* Resource table */
    if (resources == nullptr) {
        resources = new Table(100);
    }
    Resource::ref(resources);

    /* We always start with a refcount of 1 */
    count.set(1);
}

Process::~Process() {
  // if the current process had a window
  if (WindowManager::wm->windowMap.contains(id)) { 
    WindowManager::wm->removeWindow(id);
  }
  if (stack) {
    delete[] (stack - FUDGE);
    stack = 0;
  }
  if (resources) {
    Resource::unref(resources);
    resources = nullptr;
  }
}

void Process::start() {
    makeReady();
}

void Process::kill(long code) {

    Process::disable();
    if (!isKilled) {
        isKilled = true;
        killCode = code;
    }
    Process::enable();
    checkKilled();
}

bool verifyElf(Elf32_Ehdr* header) {
  if (header->e_ident[0] != ELFMAG0) return false;
  if (header->e_ident[1] != ELFMAG1) return false;
  if (header->e_ident[2] != ELFMAG2) return false;
  if (header->e_ident[3] != ELFMAG3) return false;
  if (header->e_ident[4] != ELFCLASS32) return false;
  if (header->e_ident[5] != ELFDATA2LSB) return false;
  if (header->e_ident[6] != EV_CURRENT) return false;

  if (header->e_type != ET_EXEC) return false;
  if (header->e_machine != EM_386) return false;
  if (header->e_version != EV_CURRENT) return false;

  return true;
}

long Process::execv(const char* fileName, SimpleQueue<const char*> *args, long argc) {
    //Process::trace("execv(%s, args, %d)", fileName, argc);
    File* prog = FileSystem::rootfs->rootdir->lookupFile(fileName);
    if (prog == nullptr) {
        return ERR_EXEC_NOT_FOUND;
    }

    /* Prepare address space for exec */
    addressSpace.exec();

    /* Copy args to user space */
    long userESP = 0xfffffff0;
    long* stack= (long*)userESP;
    long argMem = 0xff000000;
    char** argv = (char**)0xffff0000;

    int i = 0;


    /* read ELF */
    Elf32_Ehdr hdr;
    prog->seek(0);
    prog->readFully(&hdr,sizeof(Elf32_Ehdr));

    if (!verifyElf(&hdr)) { // if it's not a valid executable, treat as cat
      prog = FileSystem::rootfs->rootdir->lookupFile("cat");
      if (prog == nullptr) return ERR_NOT_FOUND;
      
      // load cat's elf header
      prog->seek(0);
      prog->readFully(&hdr,sizeof(Elf32_Ehdr));

      // inject cat into the args
      const char* arg = "cat";
      int argLen = K::strlen(arg);
      memcpy((void*)argMem, arg, argLen);
      ((char*)argMem)[argLen] = '\0';
      argv[i] = (char*)argMem;
      argMem += argLen + 1;
      i++;

      // doesn't make too much sense, but only print first file
      arg = args->removeHead();
      argLen = K::strlen(arg);
      memcpy((void*)argMem, arg, argLen);
      ((char*)argMem)[argLen] = '\0';
      argv[i] = (char*)argMem;
      argMem += argLen + 1;
      i++;
      delete arg;

    } else { // copy all the arguments

      while (!args->isEmpty()) {
        const char* arg = args->removeHead();
        int argLen = K::strlen(arg);

        memcpy((void*)argMem, arg, argLen);
        ((char*)argMem)[argLen] = '\0';

        argv[i] = (char*)argMem;
        argMem += argLen + 1;

        i++;
        delete arg;
      }
    }

    stack[0] = i;
    stack[1] = (long)argv;
  
    /* clear resources */
    resources->closeAll();


    uint32_t hoff = hdr.e_phoff;

    for (uint32_t i=0; i<hdr.e_phnum; i++) {
        Elf32_Phdr phdr;
        prog->seek(hoff);
        prog->readFully(&phdr,sizeof(Elf32_Phdr));
        hoff += hdr.e_phentsize;

        if (phdr.p_type == PT_LOAD) {
            char *p = (char*) phdr.p_vaddr;
            //uint32_t memsz = phdr.p_memsz;
            uint32_t filesz = phdr.p_filesz;

            prog->seek(phdr.p_offset);
            prog->readFully(p,filesz);
        }
    }

    switchToUser(hdr.e_entry, userESP,0);

    Debug::shutdown("What?");
    return 0;
}

bool Process::isTerminated() {
    return state == TERMINATED;
}

bool Process::isRunning() {
    return state == RUNNING;
}

bool Process::isBlocked() {
    return state == BLOCKED;
}

bool Process::isReady() {
    return state == READY;
}

void Process::makeReady() {
    disable();
    state = READY;
    if (this != idleProcess) {
        readyQueue->addTail(this);
    }
    enable();
}

/******************/
/* Static methods */
/******************/

void Process::vtrace(const char* msg, va_list ap) {
    Process *me = current;
    if (me != 0) {
        traceMutex->down();
    }

    Debug::printf("[");
    if (me) {
        if (me->name) {
            Debug::printf("%s",me->name);
        }
        Debug::printf("#%d",me->id);
    }
    Debug::printf("] ");
    Debug::vprintf(msg,ap);
    Debug::printf("\n");

    if (me != 0) {
        traceMutex->up();
    }
}

void Process::trace(const char* msg, ...) {
    va_list ap;
    va_start(ap,msg);
    vtrace(msg,ap);
    va_end(ap);
}

void Process::exit(long exitCode) {
    Process* p = current;

    if (p) {
        p->exitCode = exitCode;
        p->resources->closeAll();
        p->onExit();
        p->doneEvent.signal();
        
        Process::disable();
        reaperQueue->addTail(p);
        p->state = TERMINATED;
        current = nullptr;

        yield();
        Debug::panic("should never get here");
        Process::enable();
    }
}

/* switch to the next process */
void Process::dispatch(Process *prev) {
    state = RUNNING;

    uint32_t stackBottom = (uint32_t) &stack[STACK_LONGS];
    uint32_t stackBytes = stackBottom - (uint32_t) kesp;
    uint32_t stackLongs = stackBytes / 4;

    if (stackLongs >= STACK_LONGS) {
        Debug::printf("switching to %s\n",name);
        Debug::printf("switching from %s\n",prev ? prev->name : "?");
        Debug::panic("stack for %s is too big, %d bytes, iDepth:%d iCount:%d",name,stackBytes,iDepth,iCount);
    }

    if (this != prev) {
        addressSpace.activate();
        TSS::esp0((uint32_t) &stack[STACK_LONGS]);
        current = this;
        contextSwitch(
            prev ? &prev->kesp : 0, kesp, (disableCount == 0) ? (1<<9) : 0);
    }
    checkKilled();
}

void Process::yield(Queue<Process*> *q) {
    Process::disable();
    Process* me = current;
    if (me) {
        if (q) {
            /* a queue is specified, I'm blocking on that queue */
            if (me->iDepth != 0) {
                Debug::panic("blocking while iDepth = %d",me->iDepth);
            }
            me->state = BLOCKED;
            q->addTail(me);
        } else {
            /* no queue is specified, put me on the ready queue */
            me->makeReady();
        }
    }

    Process* next;

    if (readyQueue->isEmpty()) {
        if (!idleProcess) {
            idleProcess = new IdleProcess();
            idleProcess->start();
        }
        next = idleProcess;            
    } else {
        next = readyQueue->removeHead();
    }

    next->dispatch(me);
    Process::enable();
}

void Process::yield() {
    yield(nullptr);
}

/*******************/
/* The timer class */
/*******************/

class Timer {
public:
    uint32_t target;
    Timer *next;
    SimpleQueue<Process*> waiting;
};

void Process::sleepUntil(uint32_t second) {
    Process::disable();

    uint32_t target = second * Pit::hz;
    if (target > Pit::jiffies) {
        Timer **pp = &timers;
        Timer* p = timers;
        while (p) {
            if (p->target == target) {
                break;
            } else if (p->target > target) {
                p = nullptr;
                break;
            } else {
                pp = &p->next;
                p = p->next;
            }
        }
        if (!p) {
            p = new Timer();
            p->target = target;
            p->next = *pp;
            *pp = p;
        }
        Process::yield(&p->waiting);
    }

    Process::enable();
}

void Process::sleepFor(uint32_t seconds) {
    sleepUntil(Pit::seconds() + seconds);
}

/* called for every timer tick */
void Process::tick() {
    /* interrupts are already disabled but might as well */
    Process::disable();

    if (current == idleProcess) {
        idleJiffies ++;
    }

    Timer* first = timers;
    if (first) {
        if (Pit::jiffies == first->target) {
            timers = first->next;
            while (!first->waiting.isEmpty()) {
                Process* p = first->waiting.removeHead();
                p->makeReady();
            }
        }
    }

    Process::enable();
}

void Process::disable() {
    Pic::off();
    Process* me = current;
    if (me) {
        me->disableCount ++;
    }
}

void Process::enable() {
    Process* me = current;
    /* It is meaningless to enable interrupts without
       a current process */
    if (me) {
        uint32_t c = me->disableCount;
        if (c == 0) {
            Debug::panic("disable = %d",c);        
        } else if (c == 1) {
            me->disableCount = 0;
            Pic::on();
        } else {
            me->disableCount --;
        }
    }
}    

void Process::startIrq() {
    Process* me = Process::current;
    if (current == 0) {
        Debug::panic("startIrq with no process");
        return;
    }
    me->iCount ++;
    me->iDepth ++;
    if (me->disableCount != 0) {
        Debug::panic("disableCount = %d",me->disableCount);
    }
    me->disableCount = 1;
}

void Process::endIrq() {
    Process* me = Process::current;
    if (me == 0) {
        return;
    }
    me->iDepth --;
    if (me->disableCount != 1) {
        Debug::panic("disableCount = %d",me->disableCount);
    }
    me->disableCount = 0;
}
