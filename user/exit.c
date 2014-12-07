#include "libc.h"

int main(int argc, char** argv) {
  if (argc == 1) { 
    exit(0);
  } else {
    exit((int)(argv[1][0] - '0'));
  }
  return 0;
}
