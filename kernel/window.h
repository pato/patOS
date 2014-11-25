#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "vga.h"

#define MAXROWS 25
#define MAXCOLS 80

class Window {
private:
    VGA& vga;
    Frame* textBuf; //TODO: STARTHERE: create text buffer | actually just store the matrix as member
    int bg, fg;
    int row, column, height, width;
    const char* name; /* window title */
    int cRow, cCol; /* cursor row and column */
    void put(int r, int c, char ch);
    void put(int r, int c, char ch, int bg, int fg);
    void cursor(int r, int c);
    void drawTitle();
    bool boundCheck(int r, int c);
public:
    Window(VGA& vga, const char* name, int r, int c, int h, int w, int bg, int fg);
    ~Window();
    void clear();
    void fill(char c);
    void seek(int r, int c);
    void write(char c);
    void write(const char* c);
    void writeLine();
};
   
#endif
