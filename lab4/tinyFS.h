#include <bitset>
#include <iostream>
#include <unordered_map>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

#include "libDisk.h"

/* use this name for a default disk file name */
#ifndef DEFAULT_DISK_NAME
#define DEFAULT_DISK_NAME tinyFSDisk
#endif

/**
 * TinyFS superblock macros
*/
#ifndef TFS_SB_MAGIC_NUM
#define TFS_SB_MAGIC_NUM 0x5A /* The magic number for the TinyFS */
#endif
#ifndef TFS_SB_ROOTNUM_INIT
#define TFS_SB_ROOTNUM_INIT 0 /* The initial root number for the TinyFS superblock */
#endif
#ifndef TFS_SB_TOTALCT_INIT
#define TFS_SB_TOTALCT_INIT 0 /* The initial total file count for the TinyFS superblock */
#endif
#ifndef TFS_SB_MAPSIZE
#define TFS_SB_MAPSIZE 248 * 8 /* The bit-map size, in bits, in the TinyFS superblock. It uses up the rest of the space in the superblock struct. */
#endif

/**
 * TinyFS success/error code macros
*/
#ifndef SUCCESS_MKFS
#define SUCCESS_MKFS 0 /* Successfully made a file system */
#endif
#ifndef ERROR_MKFS
#define ERROR_MKFS -1000 /* Failed to make a file system */
#endif

/*****
 *      TEST VARIABLES/MACROS FOR TFS. WORK IN PROGRESS.
 *****/
#define INODE_COUNT 16
#define SUPERBLOCK_NUM 0
#define INODELIST_BLOCK_NUM 1
/* END OF TEST VARIABLES/MACROS */

typedef int fileDescriptor;

/** @brief
 * struct of dataBlock
 * 
 * 240 is even divisible by 12 (8 bytes for name, 4 for inode num)
*/
typedef struct dataBlock{
    dataBlock * nextDataBlock;
    char directDataBlock[240];
    char paddedBlock[8];

    dataBlock(){
       for(int i=0; i<8;i++){
            this->paddedBlock[i] = '\0';
       }
    }
}dataBlock;

/**
 * An inode block to keep track of metadata for each file within TinyFS.
*/
typedef struct inode
{
    int32_t f_inode; /* File inode number */
    dataBlock * first_dataBlock; // pointer to the data block of the current file
    int32_t f_size; //contains the data size of the file
    int32_t N_dataBlocks;   //contains the number of datablock information

    /* Default constructor */
    inode(){
        this->first_dataBlock = NULL;
        this->f_size = 0;
        this->f_inode = 0;
    }
}inode;

/**
 * @brief
 * struct of bitMap
*/
typedef struct bitMap{
    bitMap * nextBitMap;
    std::bitset<TFS_SB_MAPSIZE> bitmap;

    /* Default constructor */
    bitMap()
    {
        this->nextBitMap = NULL;
        
        for (int i = 0; i < TFS_SB_MAPSIZE; i++)
        {
            bitmap[i] = 0;
        }
    }
}bitMap;

/**
 * A block that stores metadata about the file system. It is always stored at 
 * logical block 0 on the disk. 
*/
typedef struct superblock
{
    int8_t sb_magicnum; /* The magic number, 0x5A, for TinyFS */
    int * sb_rootnum; //pointer to the root block
    int32_t sb_totalct; /* Total number of files in the file system */
    int32_t sb_freect; /* Number of free data blocks */

    //contains the pointer to the bitmap
    bitMap * bitMapTable; 

    /* Default constructor */
    superblock(int numBlocks){
        this->sb_magicnum = TFS_SB_MAGIC_NUM;
        this->sb_rootnum = NULL;
        this->sb_totalct = 0;
        this->sb_freect = numBlocks - 1;    //super block takes a space of one block
        auto bitMapTable = bitMap();
        this->bitMapTable = &bitMapTable;
    }
}superblock;

/**
 * TODO: add more and description and finish destructor
*/
class tfs 
{
    private:
        std::unordered_map<fileDescriptor, inode> openInodes; /* A mapping between open inodes in the file system and their associated file descriptor */
        superblock sb; /* Superblock for the TinyFS file system */
    public:
        // Constructor
        tfs(int numBlocks)
            : openInodes(std::unordered_map<fileDescriptor, inode>()), 
              sb(superblock(numBlocks)) {}

        // Destructor
        ~tfs() {}

        std::unordered_map<fileDescriptor, inode> getOpenInodeList()
        {
            return this->openInodes;
        }

        superblock* getSuperblock()
        {
            return &this->sb;
        }
};

/* Makes an empty TinyFS file system of size nBytes on an emulated libDisk
disk specified by ‘filename’. This function should use the emulated disk
library to open the specified file, and upon success, format the file to be
mountable. This includes initializing all data to 0x00, setting magic
numbers, initializing and writing the superblock and other metadata, etc.
Must return a specified success/error code. */
int tfs_mkfs(char *filename, int nBytes);

/* tfs_mount(char *filename) “mounts” a TinyFS file system located within an
emulated libDisk disk called ‘filename’. tfs_unmount(void) “unmounts” the
currently mounted file system. As part of the mount operation, tfs_mount
should verify the file system is the correct type. Only one file system may
be mounted at a time. Use tfs_unmount to cleanly unmount the currently
mounted file system. Must return a specified success/error code. */
int tfs_mount(char *filename);
int tfs_unmount(void);

/* Opens a file for reading and writing on the currently mounted file system.
Creates a dynamic resource table entry for the file (the structure that
tracks open files, the internal file pointer, etc.), and returns a file
descriptor (integer) that can be used to reference this file while the
filesystem is mounted. */
fileDescriptor tfs_open(char *name);

/* Closes the file and removes dynamic resource table entry */
int tfs_close(fileDescriptor FD);

/* Writes buffer ‘buffer’ of size ‘size’, which represents an entire file’s
contents, to the file described by ‘FD’. Sets the file pointer to 0 (the
start of file) when done. Returns success/error codes. */
int tfs_write(fileDescriptor FD, char *buffer, int size);

/* deletes a file and marks its blocks as free on disk. */
int tfs_delete(fileDescriptor FD);

/* reads one byte from the file and copies it to ‘buffer’, using the current
file pointer location and incrementing it by one upon success. If the file
pointer is already at the end of the file then tfs_readByte() should return
an error and not increment the file pointer. */
int tfs_readByte(fileDescriptor FD, char *buffer);

/* change the file pointer location to offset (absolute). Returns
success/error codes.*/
int tfs_seek(fileDescriptor FD, int offset);
