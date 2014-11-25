#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "vga.h"

#define MAXROWS 25
#define MAXCOLS 80
#define BUFFERHEIGHT (MAXROWS * 2)
#define BUFFERWIDTH MAXCOLS

struct TextBuffer {
  uint8_t data[BUFFERHEIGHT][BUFFERWIDTH][2];
  int row;       // the row into the data matrix 
  int col;       // the column into the data matrix

  TextBuffer() {
    row = 0;
    col = 0;
  }
  void write(char ch, int bg, int fg) {
    if (col < MAXCOLS) {
      data[row][col][0] = ch;
      data[row][col][1] = Frame::attrs(bg, fg);
      col++;
    } // TODO: deal with else
  }
  void writeLine() { // only real line breaks
    row = (row + 1) % BUFFERHEIGHT;
    clearRow(row);
    col = 0;
  }
  uint32_t getRealRow(int r, int height) {
    return (row - height + r) >= 0 ? (row - height + r) : BUFFERHEIGHT + ( row - height + r);
  }
  void clearRow(int row) {
    for (int i = 0; i < MAXCOLS; i++) {
      data[row][i][0] = 0;
    }
  }
};

class Window {
private:
    VGA& vga;
    TextBuffer* textBuf; //TODO: STARTHERE: create text buffer | actually just store the matrix as member
    int bg, fg;
    int row, column, height, width;
    const char* name; /* window title */
    int cRow, cCol; /* cursor row and column */
    void put(int r, int c, char ch);
    void put(int r, int c, char ch, int color);
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
    void writeLine(bool wrap);
    void redrawTextBuf();
};
   
#endif
