#include "libc.h"

/* A first-fit heap */

#define INTS 0x100000

static int array[INTS];
static int len = INTS;
static int safe = 1;
static int avail = 0;

static void makeTaken(int i, int ints);
static void makeAvail(int i, int ints);

void heap_init() {
    makeTaken(0,2);
    makeAvail(2,len-4);
    makeTaken(len-2,2);
}

static int abs(int x) {
    if (x < 0) return -x; else return x;
}

static int size(int i) {
    return abs(array[i]);
}

static int headerFromFooter(int i) {
    return i - size(i) + 1;
}

static int footerFromHeader(int i) {
    return i + size(i) - 1;
}
    
static int sanity(int i) {
    if (safe) {
        if (i == 0) return 0;
        if ((i < 0) || (i >= len)) {
//            Debug::panic("bad header index %d\n",i);
            return i;
        }
        int footer = footerFromHeader(i);
        if ((footer < 0) || (footer >= len)) {
//            Debug::panic("bad footer index %d\n",footer);
            return i;
        }
        int hv = array[i];
        int fv = array[footer];
  
        if (hv != fv) {
//            Debug::panic("bad block at %d, %d != %d\n", i, hv, fv);
            return i;
        }
    }

    return i;
}

static int left(int i) {
    return sanity(headerFromFooter(i-1));
}

static int right(int i) {
    return sanity(i + size(i));
}

static int next1(int i) {
    return sanity(array[i+1]);
}

static int prev1(int i) {
    return sanity(array[i+2]);
}

static void next(int i, int x) {
    array[i+1] = x;
}

static void prev(int i, int x) {
    array[i+2] = x;
}

static void remove(int i) {
    int prevIndex = prev1(i);
    int nextIndex = next1(i);

    if (prevIndex == 0) {
        /* at head */
        avail = nextIndex;
    } else {
        /* in the middle */
        next(prevIndex,nextIndex);
    }
    if (nextIndex != 0) {
        prev(nextIndex,prevIndex);
    }
}

static void makeAvail(int i, int ints) {
    array[i] = ints;
    array[footerFromHeader(i)] = ints;    
    next(i,avail);
    prev(i,0);
    if (avail != 0) {
        prev(avail,i);
    }
    avail = i;
}

static void makeTaken(int i, int ints) {
    array[i] = -ints;
    array[footerFromHeader(i)] = -ints;    
}

static int isAvail(int i) {
    return array[i] > 0;
}

static int isTaken(int i) {
    return array[i] < 0;
}
    
void* malloc(long bytes) {
    //Debug::printf("malloc(%d)\n",bytes);
    if (bytes == 0) return (void*) array;

    int ints = ((bytes + 3) / 4) + 2;
    if (ints < 4) ints = 4;

    int p = avail;
    sanity(p);

    void* res = 0;
    while ((p != 0) && (res == 0)) {
        if (!isAvail(p)) {
            //Debug::panic("block @ %d is not available\n",p);
        }
        int sz = size(p);
        if (sz >= ints) {
            remove(p);
            int extra = sz - ints;
            if (extra >= 4) {
                makeTaken(p,ints);
                //Debug::printf("idx = %d, sz = %d, ptr = %p\n",p,ints,&array[p+1]);
                makeAvail(p+ints,extra);
            } else {
                makeTaken(p,sz);
                //Debug::printf("idx = %d, sz = %d, ptr = %p\n",p,sz,&array[p+1]);
            }
            res = &array[p+1];
        } else {
            p = next1(p);
        }
    }
    if (res == 0) {
        //Debug::panic("heap is full, bytes=0x%x",bytes);
    }
    return res;
}        

void free(void* p) {
    if (p == 0) return;
    if (p == (void*) array) return;

    int idx = ((((long) p) - ((long) array)) / 4) - 1;
    sanity(idx);
    if (!isTaken(idx)) {
        //Debug::panic("freeing free block %p %d\n",p,idx);
        return;
    }

    int sz = size(idx);

    int leftIndex = left(idx);
    int rightIndex = right(idx);

    if (isAvail(leftIndex)) {
        remove(leftIndex);
        idx = leftIndex;
        sz += size(leftIndex);
    }

    if (isAvail(rightIndex)) {
        remove(rightIndex);
        sz += size(rightIndex);
    }

    makeAvail(idx,sz);
}

void* realloc(void* p, long newSize) {
    if (p == 0) {
        return malloc(newSize);
    }
    if (newSize == 0) {
        free(p);
        return 0;
    }
    int idx = ((((long) p) - ((long) array)) / 4) - 1;
    sanity(idx);
    if (!isTaken(idx)) {
        //Debug::panic("freeing free block %p %d\n",p,idx);
        return 0;
    }

    long sz = size(idx) * 4;

    void* newPtr = malloc(newSize);
    if (newPtr) {
        long m = (newSize > sz) ? sz : newSize;
        memcpy(newPtr,p,m);
    }    

    free(p);
    return newPtr;
}
