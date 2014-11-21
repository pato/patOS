#include "debug.h"
#include "libk.h"
#include "machine.h"
#include "semaphore.h"

OutputStream<char> *Debug::sink = 0;
bool Debug::debugAll = false;

void Debug::init(OutputStream<char> *sink) {
    Debug::sink = sink;
}

void Debug::vprintf(const char* fmt, va_list ap) {
    if (sink) {
        K::vsnprintf(*sink,1000,fmt,ap);
    }
}

void Debug::printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt,ap);
    va_end(ap);
}

void Debug::vshutdown(const char* fmt, va_list ap) {
    cli();
    vprintf(fmt,ap);
    printf("\n*** System Shutdown ***\n");
    while (1) {
        asm volatile ("hlt");
    }
}

void Debug::shutdown(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vshutdown(fmt,ap);
    va_end(ap);
}

void Debug::vpanic(const char* fmt, va_list ap) {
    printf("[PANIC] ");
    vshutdown(fmt,ap);
}

void Debug::panic(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vpanic(fmt,ap);
    va_end(ap);
}

void Debug::vdebug(const char* fmt, va_list ap) {
    if (debugAll || flag) {
       printf("[%s] ",what);
       vprintf(fmt,ap);
       printf("\n");
    }
}

void Debug::debug(const char* fmt, ...) {
    if (debugAll || flag) {
        va_list ap;
        va_start(ap, fmt);
        vdebug(fmt,ap);
        va_end(ap);
    }
}

void Debug::missing(const char* file, int line) {
    panic("Missing code at %s:%d",file,line);
}


