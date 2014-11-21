#ifndef _LIBK_H_
#define _LIBK_H_

#include <stdarg.h>
#include "io.h"
#include "stdint.h"

class K {
public:
    static void bzero(void* p, size_t n);
    static void snprintf (OutputStream<char>& sink, long maxlen, const char *fmt, ...);
    static void vsnprintf (OutputStream<char>& sink, long maxlen, const char *fmt, va_list arg);
    static long strlen(const char* str);
    static char* strdup(const char* str);
    static int isdigit(int c);
    template <typename T> static T&& move(T& x);
};

#endif
