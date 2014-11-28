#include "windowmanager.h"

#define MAXWINDOWS 6

Layout*** WindowManager::layouts;
Window* WindowManager::backdrop;
VGA WindowManager::vga;

void WindowManager::init() {
  layouts = new Layout**[MAXWINDOWS];
  layouts[0] = nullptr;
  layouts[1] = new Layout*[1] {new Layout(1,1,1,1)};
  layouts[2] = new Layout*[2] {new Layout(1,2,1,1),new Layout(1,2,1,2)};
  layouts[3] = new Layout*[3] {new Layout(1,2,1,1),new Layout(2,2,1,2),new Layout(2,2,2,2)};
  layouts[4] = new Layout*[4] {new Layout(2,2,1,1),new Layout(2,2,2,1),new Layout(2,2,1,2),new Layout(2,2,2,2)};
  layouts[5] = new Layout*[5] {new Layout(2,2,1,1),new Layout(2,2,2,1),new Layout(3,2,1,2),new Layout(3,2,2,2),new Layout(3,2,3,2)};
  layouts[6] = new Layout*[6] {new Layout(3,2,1,1),new Layout(3,2,2,1),new Layout(3,2,3,1),new Layout(3,2,1,2),new Layout(3,2,2,2),new Layout(3,2,3,2)};

  backdrop = new Window(vga, "PatOS - version 0.0.0.0.1", 0, 0, 25, 80, VGA::GREEN, VGA::WHITE);
}

void WindowManager::addWindow(const char* name, int fg) {
  addWindow(name, DEFAULTBG, fg);
}

void WindowManager::addWindow(const char* name, int bg, int fg) {
  int winCount = windowMap.getSize();
  if (winCount == MAXWINDOWS) Debug::panic("Maximum windows reached");


  backdrop->clear();
  windowMap.lock();
  MapNode<Window>* curr = windowMap.head;
  int i = 0;
  for (; curr != nullptr; i++) {
    if (i >= winCount) Debug::panic("found more windows than winCount while iterating");

    curr->value->resize(layouts[winCount + 1][i]);
    curr = curr->next;
  }
  windowMap.unlock();

  if (i != winCount) Debug::panic("i != winCount after iterating over windowMap");

  Layout* l = layouts[winCount + 1][i];
  Window* newWin = new Window(vga, name, l->r, l->c, l->h, l->w, bg, fg);

  /* If there isn't a process (because the window was added for debugging purposes */
  int id = Process::current ? Process::current->id : 666;

  windowMap.add(id, newWin);
}
