#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "stdint.h"

class BlockDevice {
public:
    const uint32_t blockSize;
    BlockDevice(uint32_t blockSize) : blockSize(blockSize) {}

    /* read the given block */
    virtual void readBlock(uint32_t blockNumber, void* buffer) = 0;

    /* read as much as count bytes starting at offset into the given buffer
       returns number of bytes actuallty read.
     */

    uint32_t read(uint32_t offset, void* buffer, uint32_t count);

    /* read count bytes starting at offset */
    void readFully(uint32_t offset, void* buffer, uint32_t count);
};

#endif
