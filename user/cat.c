#include "libc.h"

int catFile(char* fileName) {
  int fd = open(fileName);
  if (fd < 0) return fd;

  int len = getlen(fd);
  char buffer[len + 1];

  readFully(fd, buffer, len);
  buffer[len] = '\0';

  puts(buffer);
  return 0;  
}

int main(int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    int e = catFile(argv[i]);    
    if (e < 0) return e;
  }
  return 0;
}
