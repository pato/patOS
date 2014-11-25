#include "vga.h"
#include "machine.h"


int VGA::BLACK = 0;
int VGA::BLUE = 1;
int VGA::GREEN = 2;
int VGA::CYAN = 3;
int VGA::RED = 4;
int VGA::MAGENTA = 5;
int VGA::BROWN = 6;
int VGA::LIGHT_GRAY = 7;
int VGA::DARK_GRAY = 8;
int VGA::LIGHT_BLUE = 9;
int VGA::LIGHT_GREEN = 10;
int VGA::LIGHT_CYAN = 11;
int VGA::LIGHT_RED = 12;
int VGA::LIGHT_MAGENTA = 13;
int VGA::LIGHT_BROWN = 14;
int VGA::WHITE = 15;


static Frame *frame = (Frame*) 0xb8000;
//static Frame *frame = (Frame*) 0xA0000;

void VGA::cursor(int row, int col, int bg, int fg) {
    int next = (row*80 + col);

    outb(0x3d4,0xf);
    outb(0x3d5,next);
    outb(0x3d4,0xe);
    outb(0x3d5,next>>8);
    frame->data[row][col][1] = Frame::attrs(bg,fg);
}

void VGA::put(int r, int c, char ch, int bg, int fg) {
    frame->data[r][c][0] = ch;
    frame->data[r][c][1] = Frame::attrs(bg,fg);
}
void VGA::put(int r, int c, char ch, int color) {
    frame->data[r][c][0] = ch;
    frame->data[r][c][1] = color;
}

