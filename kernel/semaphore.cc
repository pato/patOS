#include "semaphore.h"
#include "process.h"
#include "atomic.h"
#include "pic.h"

Semaphore::Semaphore(int count) : Resource(ResourceType::SEMAPHORE),
    count(count), waiting()
{
}

Semaphore::~Semaphore() {
}

void Semaphore::down() {
    Process::disable();
    if (count == 0) {
        if (Process::current == 0) {
            Debug::panic("blocking without process");
        }
        Process::yield(&waiting); // yield unlock the queue
    } else {
        count --;
    }
    Process::enable();
}

void Semaphore::up() {
    Process::disable();
    if (waiting.isEmpty()) {
        count ++;
    } else {
        Process *p = waiting.removeHead();
        p->makeReady();
    }
    Process::enable();
}
