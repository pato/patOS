#include "libc.h"

struct fileEntry {
  char name[12];
  unsigned int start;
};


int main() {
  int fd = open(".");
  int len = getlen(fd);

  char buffer[len];
  readFully(fd, &buffer, len);

  int fileCount = (len / sizeof(struct fileEntry));
  
  struct fileEntry* files = (struct fileEntry*)buffer;

  for (int i =0 ; i < fileCount; i++) {
    puts(files[i].name);
    puts("\n");
  }


  return 0;
}
