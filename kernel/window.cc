/** Works Cited **
 * Learned about the constructor syntax
 *  http://www.cplusplus.com/doc/tutorial/classes/
 */

#include "window.h"
#include "machine.h"

Window::Window(VGA& vga, int r, int c, int h, int w, int bg, int fg)
  : vga(vga) {

  this->row = r;
  this->column = c;
  this->height = h;
  this->width = w;

  this->bg = bg;
  this->fg = fg;
}

void Window::clear() {
  for (uint8_t c = 0; c < this->width; c++) {
    for (uint8_t r = 0; r < this->height; r++) {
      Window::put(r, c, ' ');
    }
  }
}

void Window::fill(char ch) {
    for (int r=0; r<100; r++) {
        for (int c=0; c<100; c++) {
            Window::put(r,c,ch);
        }
    }
}

void Window::put(int r, int c, char ch) {
  // Ensure that the we really only write to this window
  if (r < 0 || c < 0 || r >= this->height || c >= this->width) {
    return;
  }
  this->vga.put(this->row + r, this->column + c, ch, this->bg, this->fg); 
}
