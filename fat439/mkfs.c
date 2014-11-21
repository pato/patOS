#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/mman.h>

typedef struct {
    char magic[4];
    uint32_t nBlocks;
    uint32_t avail;
    uint32_t root;
} Super;

Super* super;
uint32_t *fat;
char* blocks;

void* mapStart;
size_t mapLength;

uint32_t getBlock() {
    uint32_t idx = super->avail;
    if (idx == 0) {
        fprintf(stderr,"disk is full\n");
        exit(-1);
    }
    super->avail = fat[idx];
    fat[idx] = 0;
    return idx;
}

char* toPtr(uint32_t idx, uint32_t offset) {
    return blocks + idx * 512 + offset;
}

uint32_t min(uint32_t a, uint32_t b) {
    return (a < b) ? a : b;
}

uint32_t oneFile(const char* fileName) {
    int fd = open(fileName,O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(-1);
    }

    uint32_t startBlock = getBlock();
    uint32_t *fileMetaData = (uint32_t*) toPtr(startBlock,0);
    fileMetaData[0] = 1;

    uint32_t currentBlock = startBlock;
    uint32_t leftInBlock = 512 - 8;
    uint32_t blockOffset = 8;
    uint32_t totalSize = 0;

    while (1) {
        if (leftInBlock == 0) {
            uint32_t b = getBlock();
            fat[currentBlock] = b;
            currentBlock = b;
            blockOffset = 0;
            leftInBlock = 512;
        }
        ssize_t n = read(fd,toPtr(currentBlock,blockOffset),leftInBlock);
        if (n < 0) {
            perror("read");
            exit(-1);
        } else if (n == 0) {
            /* eof */
            fileMetaData[1] = totalSize;
            break;
        } else {
            /* read some */
            blockOffset += n;
            leftInBlock -= n;
            totalSize += n;
        }
    }

    close(fd);

    return startBlock;
}

int main(int argc, const char *argv[]) {
    if (argc < 4) {
        fprintf(stderr,"usage: %s <image name> <nBlocks> <file0> <file1> ...\n",argv[0]);
        exit(-1);
    }

    const char* fileName = argv[1];
    int nBlocks = atoi(argv[2]);
    const char **fileNames = &argv[3];
    int nFiles = argc - 3;

    int fd = open(fileName, O_CREAT | O_RDWR , 0666);
    if (fd == -1) {
        perror("create");
        exit(-1);
    }

    mapLength = nBlocks * 512;

    int rc = ftruncate(fd,mapLength);
    if (rc == -1) {
        perror("truncate");
        exit(-1);
    }

    mapStart = mmap(0,mapLength,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0);
    if (mapStart == MAP_FAILED) {
        perror("mmap");
        exit(-1);
    }
    super = (Super*) mapStart;
    blocks = (char*) mapStart;
    fat = (uint32_t*) (blocks + 512);
    uint32_t fatBlocks = ((nBlocks * sizeof(uint32_t)) + 511) / 512;

    super->magic[0] = 'F';
    super->magic[1] = '4';
    super->magic[2] = '3';
    super->magic[3] = '9';
    super->nBlocks = nBlocks;
    super->avail = nBlocks - 1;

    uint32_t lastAvail = 1 + fatBlocks;

    for (uint32_t i=super->avail; i>lastAvail; i--) {
        fat[i] = i-1;
    }

    /* root direcotry */
    super->root = getBlock();
    uint32_t *rootMetaData = (uint32_t*) toPtr(super->root,0);
    rootMetaData[0] = 2;
    rootMetaData[1] = nFiles * 16;
    char* rootData = toPtr(super->root,8);

    /* iterate over files */
    for (int i=0; i<nFiles; i++) {
        uint32_t x = oneFile(fileNames[i]);
        char* nm = strdup(fileNames[i]);
        char* base = basename(nm);
        char* dest = strncpy(rootData + i * 16, base, 12);
        free(nm);
        *((uint32_t*)(dest + 12)) = x;
    }

    munmap(mapStart,mapLength);

    return 0;
}
