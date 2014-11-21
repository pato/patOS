#include "pic.h"
#include "machine.h"
#include "idt.h"
#include "pit.h"
#include "debug.h"
#include "process.h"
#include "kbd.h"

#define C1 0x20           /* command port for PIC1 */
#define D1 (C1 + 1)       /* data port for PIC1 */
#define C2 0xA0           /* command port for PIC2 */
#define D2 (C2 + 1)       /* data port for PIC2 */

int firstIdt = 0x30;

void Pic::init() {

    /* ICW1 */
    outb(C1,0x11);        /* init with ICW4, not single */
    outb(C2,0x11);        /* init with ICW4, not single */

    /* ICW2 */
    outb(D1,firstIdt);    /* IDT index for IRQ0 */
    outb(D2,firstIdt+8);  /* IDT index for IRQ8 */

    /* ICW3 */
    outb(D1, 1 << 2);     /* tells master that the save is at IRQ2 */
    outb(D2, 2);          /* tells salve that it's connected at IRQ2 */

    /* ICW4 */
    outb(D1, 1);          /* 8086 mode */
    outb(D2, 1);          /* 8086 mode */

    /* enable all */
    outb(D1,0);
    outb(D2,0);

    IDT::addInterruptHandler(firstIdt +  0, (uint32_t) irq0);
    IDT::addInterruptHandler(firstIdt +  1, (uint32_t) irq1);
    IDT::addInterruptHandler(firstIdt +  2, (uint32_t) irq2);
    IDT::addInterruptHandler(firstIdt +  3, (uint32_t) irq3);
    IDT::addInterruptHandler(firstIdt +  4, (uint32_t) irq4);
    IDT::addInterruptHandler(firstIdt +  5, (uint32_t) irq5);
    IDT::addInterruptHandler(firstIdt +  6, (uint32_t) irq6);
    IDT::addInterruptHandler(firstIdt +  7, (uint32_t) irq7);
    IDT::addInterruptHandler(firstIdt +  8, (uint32_t) irq8);
    IDT::addInterruptHandler(firstIdt +  9, (uint32_t) irq9);
    IDT::addInterruptHandler(firstIdt +  10, (uint32_t) irq10);
    IDT::addInterruptHandler(firstIdt +  11, (uint32_t) irq11);
    IDT::addInterruptHandler(firstIdt +  12, (uint32_t) irq12);
    IDT::addInterruptHandler(firstIdt +  13, (uint32_t) irq13);
    IDT::addInterruptHandler(firstIdt +  14, (uint32_t) irq14);
    IDT::addInterruptHandler(firstIdt +  15, (uint32_t) irq15);
}

int Pic::irr() {
    outb(C1,0xa);
    outb(C2,0xa);
    return (inb(C2) << 8) + inb(C1);
}

int Pic::isr() {
    outb(C1,0xb);
    outb(C2,0xb);
    return (inb(C2) << 8) + inb(C1);
}

int Pic::imr() {
    return (inb(D2) << 8) + inb(D1);
}

// disable interrupts
void Pic::off() {
    cli();
}

// enable interrupts
void Pic::on() {
    sti();
}

void pic_eoi(int irq) {
    if (irq >= 8) {
        /* let PIC2 know */
        outb(C2,0x20);
    }
    /* we always let PIC1 know because PIC2 is routed though PIC1 */
    outb(C1,0x20);
}

extern "C" void pic_irq(int irq) {
    Process::startIrq();
    switch (irq) {
    case 0: Pit::handler(); break;
    case 1: /*Keyboard::handler();*/ break;
    case 4: /*com1 */ break;
    case 15: /* ide */ break;
    default: Debug::printf("interrupt %d\n",irq);
    }
    pic_eoi(irq); /* the PIC can deliver the next interrupt,
                     but interrupts are still disabled */
    Process::yield();
    Process::endIrq();
}
