#include "windowmanager.h"

/**
 * Assumptions: 
 * splits is the number of windows accross that axis (starts at 1)
 * which is the index of the window (also starts at 1)
 *
 */

Layout*** WindowManager::layouts;

void WindowManager::init() {
  layouts = new Layout**[5];
  layouts[0] = nullptr;
  layouts[1] = new Layout*[1] { new Layout(1,1) };
  layouts[2] = new Layout*[2] { new Layout(2,1), new Layout(2,2)};
}

void WindowManager::addWindow(const char* name, int fg) {
  int winCount = windowMap.getSize();

  windowMap.lock();
  MapNode<Window>* curr = windowMap.head;
  for (int i = 0; curr != nullptr; i++) {
    if (i >= winCount) Debug::panic("found more windows while iterating than winCount");

    curr->value->resize(layouts[winCount + 1][i]);
    curr = curr->next;
  }
  windowMap.unlock();
}
