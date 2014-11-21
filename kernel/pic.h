#ifndef _PIC_H_
#define _PIC_H_

class Pic {
public:
    static void init();
    static int isr(void);
    static int irr(void);
    static int imr(void);
    static void on();
    static void off();
};

#endif
