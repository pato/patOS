#include "window.h"
#include "machine.h"


Window::Window(VGA& vga, const char* name, int r, int c, int h, int w, int bg, int fg)
  : vga(vga) {

  this->name = name;
  this->row = r;
  this->column = c;
  this->height = h;
  this->width = w;

  this->bg = bg;
  this->fg = fg;

  this->cRow = 1;
  this->cCol = 0;

  this->textBuf = new TextBuffer();
  Window::clear();
}

Window::~Window() {
  delete textBuf;
}

bool Window::boundCheck(int r, int c) {
  // Ensure that the we really only write to this window
  return !(r < 0 || c < 0 || r >= this->height || c >= this->width);
}

void Window::clear() {
  for (uint8_t c = 0; c < this->width; c++) {
    for (uint8_t r = 0; r < this->height; r++) {
      Window::put(r, c, ' ');
    }
  }
  drawTitle();
}

void Window::fill(char ch) {
    for (int r=0; r<100; r++) {
        for (int c=0; c<100; c++) {
            Window::put(r,c,ch);
        }
    }
}

void Window::drawTitle() {
  int i = 0;
  for (; name[i] != '\0'; i++)
    Window::put(0, i, name[i], TITLEBG, TITLEFG);
  for (; i < width; i++) 
    Window::put(0, i, ' ', TITLEBG, TITLEFG);
}

void Window::seek(int r, int c) {
  if (!boundCheck(r, c)) return;
  this->cRow = r;
  this->cCol = c;
}

void Window::put(int r, int c, char ch, int bg, int fg) {
  this->vga.put(this->row + r, this->column + c, ch, bg, fg); 
}

void Window::put(int r, int c, char ch, int color) {
  this->vga.put(this->row + r, this->column + c, ch, color); 
}

void Window::put(int r, int c, char ch) {
  if (!boundCheck(r, c)) return;
  this->vga.put(this->row + r, this->column + c, ch, this->bg, this->fg); 
}

void Window::cursor(int r, int c) {
  if (!boundCheck(r, c)) return;
  this->vga.cursor(this->row + r, this->column + c, this->bg, this->fg);
}

void Window::write(const char* str) {
  for (int i = 0; str[i] != '\0'; i++) {
    write(str[i]);
  }
}

void Window::write(char c) {
  if (cCol + 1 > width) {
    writeLine(true);
  } else {
    cursor(cRow, cCol + 1);
    put(cRow, cCol, c);
    textBuf->write(c, bg, fg);
    cCol++;
  }
}

void Window::writeLine(bool wrap) {
  if (!wrap) textBuf->writeLine(); // if it's a real line break, write it to text buffer
  if (cRow + 1 > (height - 1)) {
    redrawTextBuf();
    for (int j = 0; j<width; j++) put(cRow, j, ' ', bg, fg);
    cCol = 0;
  } else {
    cRow++;
    cCol = 0;
  }
}

void Window::redrawTextBuf() {
  // TODO: make it draw lines longer than width
  for (int i = 1; i < height; i++) {
    int j = 0;
    for (; j < width; j++) {
      uint32_t realRow = textBuf->getRealRow(i, height);
      char c = textBuf->data[realRow][j][0];
      uint8_t color = textBuf->data[realRow][j][1];
      if (c == 0) break;
      put(i, j, c, color);
    }
    for (;j<width;j++) put(i, j, ' ', bg, fg);
  }
}
