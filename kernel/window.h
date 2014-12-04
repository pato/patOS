#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "vga.h"
#include "layout.h"
#include "debug.h"

#define MAXROWS 25 /* also defined in layout.h */
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
    } // TODO: deal with else, might want to support longer lines, but can also define as limitation
  }
  void writeLine() { // only real line breaks
    data[row][col][0] = '\0';
    row = (row + 1) % BUFFERHEIGHT;
    clearRow(row);
    col = 0;
  }
  /**
   * get the row into the text buffer
   * r - the relative row of the window we are displaying
   * height - the height of the window where we are displaying
   */
  uint32_t getRealRow(int r, int height) {
    r += 1; // compensate for the titlebar row
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
    TextBuffer* textBuf;
    bool full;
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
    void resize(int r, int c, int h, int w);
    void resize(Layout* layout);
    void redrawTextBuf();
};
   
#endif
