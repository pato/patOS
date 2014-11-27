#ifndef _WINDOWMANAGER_H_
#define _WINDOWMANAGER_H_

#include "map.h"
#include "layout.h"

#define DEFAULTBG VGA::BLUE


class WindowManager {
  private:
    static Layout*** layouts;
  public:
    VGA vga;
    Map<Window> windowMap;
    void init();
    void addWindow(const char* name, int fg);
};

#endif
