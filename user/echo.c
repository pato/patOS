#include "libc.h"

int main(int argc, char** argv) {
    char *sep = "";
    for (int i=1; i<argc; i++) {
        puts(sep);
        sep = " ";        
        puts(argv[i]);
    }
    puts("\n");
    return 0;
}
