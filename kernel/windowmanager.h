#ifndef _WINDOWMANAGER_H_
#define _WINDOWMANAGER_H_

#include "map.h"
#include "layout.h"

#define DEFAULTBG VGA::BLUE


class WindowManager {
  private:
    static VGA vga;
    static Layout*** layouts;
    static Window* backdrop;
  public:
    static WindowManager* wm;

    Map<Window> windowMap;
    static void init();
    Window* currentWindow();
    void shiftFocus(int window);
    void addWindow(const char* name, int fg);
    void addWindow(const char* name, int bg, int fg);
};

#endif
