#include "window.h"

Window::Window(VGA& vga, const char* name, int r, int c, int h, int w, int bg, int fg, int pos)
  : vga(vga) {

  this->name = K::strdup(name);
  this->row = r;
  this->column = c;
  this->height = h;
  this->width = w;

  this->bg = bg;
  this->fg = fg;

  this->cRow = 1;
  this->cCol = 0;

  this->full = false;

  this->textBuf = new TextBuffer();
  this->focus = new Event();
  
  this->pos = pos;

  Window::clear();
}

Window::~Window() {
  delete textBuf;
  delete focus;
}

static char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

bool Window::boundCheck(int r, int c) {
  // Ensure that the we really only write to this window
  return !(r < 0 || c < 0 || r >= this->height || c >= this->width);
}

void Window::clear() {
  fill(' ');
  drawTitle();
}

void Window::fill(char ch) {
  for (uint8_t c = 0; c < this->width; c++) {
    for (uint8_t r = 0; r < this->height; r++) {
      Window::put(r, c, ch);
    }
  }
}

void Window::drawTitle() {
  drawTitle(pos);
}

void Window::drawTitle(int n) {
  int c = 0;
  if ( pos >= 0) {
    put(0, c++, hexDigits[n], TITLEBG, TITLEFG);
    put(0, c++, ' ', TITLEBG, TITLEFG);
    put(0, c++, '-', TITLEBG, TITLEFG);
    put(0, c++, ' ', TITLEBG, TITLEFG);
  }
  int i = 0;
  for (; name[i] != '\0' && i < (width - c); i++)
    Window::put(0, c+i, name[i], TITLEBG, TITLEFG);
  for (; i < (width - c); i++) 
    Window::put(0, c+i, ' ', TITLEBG, TITLEFG);
}

void Window::seek(int r, int c) {
  if (!boundCheck(r, c)) return;
  this->cRow = r;
  this->cCol = c;
  cursor(r,c);
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

void Window::updatePos(int p) {
  pos = p;
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

void Window::setFgColor(int color) {
  if (color > 15 || color < 0) return;
  fg = color;
}

void Window::drawCursor() {
  cursor(cRow, cCol);
}

void Window::writeLine(bool wrap) {
  if (!wrap) textBuf->writeLine(); // if it's a real line break, write it to text buffer
  if (cRow + 1 > (height - 1)) {
    redrawTextBuf();
    full = true;
    for (int j = 0; j<width; j++) put(cRow, j, ' ', bg, fg);
    cCol = 0;
  } else {
    cRow++;
    cCol = 0;
  }
}

void Window::resize(int r, int c, int h, int w) {
  this->row = r;
  this->column = c;
  this->width = w;
  this->height = h;
  redrawTextBuf();
}
void Window::resize(Layout* l) {
  resize(l->r, l->c, l->h, l->w);
}

void Window::redrawTextBuf() {
  clear();
  // TODO: make it draw lines longer than width
  int i = 1, j = 0;
  int maxHeight = full ? height : cRow + 1;
  for (i = 1; i < maxHeight; i++) {
    j = 0;
    for (; j < width; j++) {
      uint32_t realRow = textBuf->getRealRow(i, maxHeight);
      char c = textBuf->data[realRow][j][0];
      uint8_t color = textBuf->data[realRow][j][1];
      if (c == 0) break;
      put(i, j, c, color);
    }
    for (;j<width;j++) put(i, j, ' ', bg, fg);
  }
}
