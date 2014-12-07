#include "libc.h"
#include "sys.h"

static char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

void puts(char* p) {
    char c;
    while ((c = *p++) != 0) putchar(c); 
}

char* gets() {
    long sz = 0;
    char *p = 0;
    long i = 0;

    while (1) {
        if (i >= sz) {
            sz += 10;
            p = realloc(p,sz+1);
            if (p == 0) return 0;
        }
        char c = getchar();
        if (c == 8) { 
          if (i > 0) {
            p[i--] = 127;
            continue;
          }
        } else {
          putchar(c);
        }
        if (c == 13) {
            puts("\n");
            p[i] = 0;
            return p;
        }
        p[i++] = c;        
    }
}
        
void puthex(long v) {
    for (int i=0; i<sizeof(long)*2; i++) {
          char c = hexDigits[(v & 0xf0000000) >> 28];
          putchar(c);
          v = v << 4;
    }
}

void putdec(unsigned long v) {
    if (v >= 10) {
        putdec(v / 10);
    }
    putchar(hexDigits[v % 10]);
}

long readFully(long fd, void* buf, long length) {
    long togo = length;
    char* p = (char*) buf;
    while(togo) {
        long cnt = read(fd,p,togo);
        if (cnt < 0) return cnt;
        if (cnt == 0) return length - togo;
        p += cnt;
        togo -= cnt;
    }
    return length;
}

long strlen(const char* str) {
    long n = 0;
    while (*str++ != 0) n++;
    return n;
}

/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/* ANSI sez:
 *   The `strcmp' function compares the string pointed to by `s1' to the
 *   string pointed to by `s2'.
 *   The `strcmp' function returns an integer greater than, equal to, or less
 *   than zero, according as the string pointed to by `s1' is greater than,
 *   equal to, or less than the string pointed to by `s2'.  [4.11.4.2]
 */
long strcmp(const char *s1, const char *s2) {
  for ( ; *s1 == *s2; s1++, s2++)
    if (*s1 == '\0')
      return 0;
  return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}
