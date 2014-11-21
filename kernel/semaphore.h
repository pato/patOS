#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "queue.h"
#include "resource.h"

class Process;

class Semaphore : public Resource {
    int count;
    SimpleQueue<Process*> waiting;
public:
    Semaphore(int count);
    virtual ~Semaphore();
    void down();
    void up();
};

class Mutex : Semaphore {
public:
    Mutex() : Semaphore(1) {}
    void lock() { down(); }
    void unlock() { up(); }
};

class Event {
    Semaphore status;
public:
    Event() : status(0) {}
    void wait() {
        status.down();
        status.up();
    }
    void signal() {
        status.up();
    }
};

#endif
