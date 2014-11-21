#include "child.h"
#include "machine.h"
#include "err.h"

Child::Child(Process *parent) : Process("child",parent->resources->forkMe()) {
    parent->addressSpace.fork(&addressSpace);
}

long Child::run() {
    switchToUser(pc,esp,0);
    return ERR_NOT_POSSIBLE;
}
