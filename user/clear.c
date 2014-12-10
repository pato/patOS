#include "libc.h"
#include "libwin.h"

int main() {
  int height = win_height();
  for (int i=0;i<height;i++) puts("\n");
  return 0;
}
