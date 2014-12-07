#include "libc.h"

int main(int argc, char** argv) {
  if (argc == 1) { 
    exit(0);
  } else {
    puts("about to exit with status code: ");
    putdec((long)argv[1] - '0');
    //exit((long)argv[1] - '0');
  }
  return 0;
}
