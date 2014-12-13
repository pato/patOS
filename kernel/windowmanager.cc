#include "windowmanager.h"

#define MAXWINDOWS 6
#define DEBUGON 0

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

  backdrop = new Window(vga, "PatOS - version 0.1.0", 0, 0, 25, 80, VGA::GREEN, VGA::WHITE, -1);
}

Window* WindowManager::currentWindow() {
  Window* currWindow = windowMap.get(Process::current->id);
  if (!currWindow) { /* Maybe you are a child of a window */
    currWindow = windowMap.get(Process::current->windowId);
  }
  return currWindow;
}

void resetFocus(Window* win) {
  if (win->focus->isSignaled()) {
    /* may need to revisit the event signal because it may not be thread safe */
    delete win->focus;
    win->focus = new Event();
    if (DEBUGON) Debug::cprintf("resetFocus---\n");
  }
}

void giveFocus(Window* win) {
  win->focus->signal();
}

void WindowManager::shiftFocus(int window) {
  if (DEBUGON) Debug::cprintf("---shiftFocus(%d)\n", window);
  int winCount = windowMap.getSize();
  if (window > winCount) return;

  //if (DEBUGON) Debug::cprintf("winCount: %d\n", winCount);
  windowMap.lock();
  MapNode<Window>* curr = windowMap.head;
  int i = 0;
  for (; curr != nullptr; i++) {
    if (i >= winCount) Debug::panic("found more windows than winCount while iterating");
    if (DEBUGON) Debug::cprintf("i: %d\n", i);
    
    if (window == curr->value->pos) {
      giveFocus(curr->value);
      curr->value->drawCursor();
      Debug::cprintf("focused window: %d height: %d width: %d\n", curr->value->pos, curr->value->height, curr->value->width);
      curr->value->debug();
      if (DEBUGON) Debug::cprintf("::giveFocus(%d)\n", i);
    } else {
      resetFocus(curr->value);
      if (DEBUGON) Debug::cprintf("::resetFocus(%d)\n", i);
    }
    curr = curr->next;
  }
  windowMap.unlock();
  Keyboard::bb->put('\1');
  if (DEBUGON) Debug::cprintf("---done shiftFocus\n");

}

void WindowManager::removeWindow(int processId) {
  Window* window = windowMap.get(processId);
  if (!window) Debug::panic("trying to remove a nonexistant window. pid: %d", processId);
  windowMap.remove(processId);
  delete window;
  redrawWindows(true);
  giveFocus(windowMap.head->value);
  windowMap.head->value->drawCursor();
}

void WindowManager::redrawWindows(bool reset) {
  backdrop->clear();

  windowMap.lock();
  int winCount = windowMap.getSize();
  if (winCount == MAXWINDOWS) Debug::panic("Maximum windows reached");

  MapNode<Window>* curr = windowMap.head;
  int i = 0;
  for (; curr != nullptr; i++) {
    if (i >= winCount) Debug::panic("found more windows than winCount while iterating");

    curr->value->updatePos(i+1);
    curr->value->resize(layouts[winCount][i]);
    if (reset) resetFocus(curr->value);
    curr = curr->next;
  }
  windowMap.unlock();
}

void WindowManager::addWindow(const char* name, int fg) {
  addWindow(name, DEFAULTBG, fg);
}

void WindowManager::addWindow(const char* name, int bg, int fg) {
  if (DEBUGON) Debug::cprintf("addWindow(%s, %d, %d)\n", name, bg, fg);
  // clear the screen by redrawing the backdrop
  backdrop->clear();

  windowMap.lock();
  int winCount = windowMap.getSize();
  if (winCount == MAXWINDOWS) Debug::panic("winCount == MAXWINDOWS");

  MapNode<Window>* curr = windowMap.head;
  int i = 0;
  for (; curr != nullptr; i++) {
    if (i >= winCount) Debug::panic("found more windows than winCount while iterating");

    curr->value->updatePos(i+1);
    curr->value->resize(layouts[winCount + 1][i]);
    resetFocus(curr->value);
    curr = curr->next;
  }
  windowMap.unlock();

  if (i != winCount) Debug::panic("i != winCount after iterating over windowMap");

  Layout* l = layouts[winCount + 1][i];
  Window* newWin = new Window(vga, name, l->r, l->c, l->h, l->w, bg, fg, i+1);

  /* If there isn't a process (because the window was added for debugging purposes */
  int id = Process::current ? Process::current->id : 666;

  newWin->focus->signal();

  windowMap.add(id, newWin);

  Keyboard::bb->put('\1');

  newWin->drawCursor();
}
