#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "vga.h"

class Window {
private:
    VGA& vga;
    int bg, fg;
    int row, column, height, width;
    const char* name; /* window title */
    int cRow, cCol; /* cursor row and column */
public:
    Window(VGA& vga, int r, int c, int h, int w, int bg, int fg);
    void put(int r, int c, char ch);
    void clear();
    void fill(char c);
};
   
#endif
