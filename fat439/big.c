#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

void writeFully(int fd, void* buf, size_t length) {
    size_t togo = length;
    char* p = (char*) buf;
    while (togo) {
        size_t cnt = write(fd,p,togo);
        if (cnt <= 0) { perror("write"); exit(-1); }
        p += cnt;
        togo -= cnt;
    }
}

int main() {
     int fd = creat("big.data",0666);
     if (fd < 0) {
         perror("open");
         exit(-1);
     }

     for (int i=0; i<3000; i++) {
         writeFully(fd,&i,sizeof(int));
     }

     close(fd);
     return 0;
}
