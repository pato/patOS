#ifndef _IDE_H_
#define _IDE_H_

#include "block.h"
#include "semaphore.h"

class IDE : public BlockDevice {
    int drive;
    Mutex mutex;
public:
    static constexpr uint32_t SECTOR_SIZE = 512;
    IDE(int drive) : BlockDevice(SECTOR_SIZE), drive(drive) {}

    void readBlock(uint32_t blockNumber, void* buffer);
};

#endif
