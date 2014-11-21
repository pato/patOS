#ifndef _FS_H_
#define _FS_H_

#include "block.h"
#include "resource.h"
#include "semaphore.h"

/****************/
/* File systems */
/****************/

class FileSystem;

/* A file */
class File : public Resource {
public:
    FileSystem* fs;
    uint32_t offset;

    File(FileSystem *fs) : Resource(ResourceType::FILE), fs(fs), offset(0) {}
    virtual ~File() {}

    /* We can ask it for its type */
    virtual uint32_t getType() = 0;    /* 1 => directory, 2 => file */
    /* We can ask it for its length in bytes */
    virtual uint32_t getLength() = 0;

    virtual void seek(uint32_t offset) {
        this->offset = offset;
    }

    /* We can read a few bytes, returned value:
         < 0 => error
         0 => end of file
         >0 => number of bytes read

         it's not an error to get less bytes than we've asked for
    */
    virtual int32_t read(void *buf, uint32_t length) = 0;

    /* read as many bytes as you can, returned value:

        < 0 => error
        n   => how many bytes were read

        n < length => end of file reached after n bytes
    */
    int32_t readFully(void* buf, uint32_t length) {
        uint32_t togo = length;
        char* ptr = (char*) buf;

        while (togo > 0) {
            int32_t c = read(ptr,togo);
            if (c < 0) return c;
            if (c == 0) return length - togo;
            togo -= c;
            ptr += c;
        }

        return length;
    }

};

/* A directory */
class Directory {
public:
    FileSystem* fs;
    Directory(FileSystem *fs) : fs(fs) {}
    virtual File* lookupFile(const char* name) = 0;
    virtual Directory* lookupDirectory(const char *name) = 0;
};

class FileSystem {
public:
     static FileSystem *rootfs;    // the root file system
     static void init(FileSystem *rfs);

     BlockDevice *dev;
     Directory *rootdir;           // the root directory

     FileSystem(BlockDevice *dev) : dev(dev) {}
};

/**************************/
/* The FAT439 file system */
/**************************/

class OpenFile;
typedef OpenFile *OpenFilePtr;

class Fat439 : public FileSystem {
public:
    struct {
        char magic[4];
        uint32_t nBlocks;
        uint32_t avail;
        uint32_t root;
    } super;
    uint32_t *fat;
    Mutex openFilesMutex;
    OpenFilePtr *openFiles;
    Fat439(BlockDevice *dev);
    OpenFile* openFile(uint32_t start);
    void closeFile(OpenFile* of);
};

#endif
