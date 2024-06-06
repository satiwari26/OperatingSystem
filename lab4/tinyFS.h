#include <bitset>
#include <iostream>
#include <unordered_map>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "libDisk.h"

/* use this name for a default disk file name */
#ifndef DEFAULT_DISK_NAME
#define DEFAULT_DISK_NAME "tinyFSDisk"
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
#define TFS_SB_MAPSIZE (248 * 8) /* The bit-map size, in bits, in the TinyFS superblock. It uses up the rest of the space in the superblock struct. */
#endif

/**
 * TinyFS success/error code macros
*/
#ifndef SUCCESS_TFS_MKFS
#define SUCCESS_TFS_MKFS 0 /* Successfully made a file system */
#endif
#ifndef SUCCESS_TFS_MOUNT
#define SUCCESS_TFS_MOUNT 0 /* Successfully mounted a file system */
#endif
#ifndef ERROR_TFS_MKFS
#define ERROR_TFS_MKFS -1000 /* Failed to make a file system */
#endif
#ifndef ERROR_TFS_MOUNT
#define ERROR_TFS_MOUNT -1001 /* Failed to make a file system */
#endif
#ifndef ERROR_TFS_UNMOUNT
#define ERROR_TFS_UNMOUNT -1002
#endif

/*****
 *      TEST VARIABLES/MACROS FOR TFS. WORK IN PROGRESS.
 *****/
#define INODE_COUNT 16
#define SUPERBLOCK_NUM 0
#define INODELIST_BLOCK_NUM 1
#define MAX_FILENAME_LEN 8
/* END OF TEST VARIABLES/MACROS */

typedef int fileDescriptor;

/** @brief
 * struct of dataBlock
 * 
 * 240 is even divisible by 12 (8 bytes for name, 4 for inode num)
*/
typedef struct dataBlock {
    dataBlock *nextDataBlock;
    char directDataBlock[240];
    char paddedBlock[8];

    /* Default constructor */
    dataBlock() {
        this->nextDataBlock = NULL;

        for (int i = 0; i < 240; i++) {
            this->directDataBlock[i] = '\0'; // Data blocks are initially empty
        }

        for(int i = 0; i < 8; i++) {
            this->paddedBlock[i] = '\0'; // Null padding to make the struct 256 bytes
        }
        std::cout << "size datablock:" << sizeof(dataBlock) << std::endl;
    }


}__attribute__((__packed__)) dataBlock; // Must pack to avoid compiler alignments changing the size of the struct (to maintain 256 bytes)

/**
 * An inode block to keep track of metadata for each file within TinyFS.
*/
typedef struct inode
{
    int32_t f_inode; /* File inode number */
    dataBlock *first_dataBlock; // pointer to the data block of the current file
    int32_t f_size; // contains the data size of the file
    int32_t N_dataBlocks;   // contains the number of datablock information
    char paddedBlock[236];

    /* Default constructor, typically for root inode */
    inode(){
        this->f_inode = 0; /* Root inode is stored at number 0 */
        this->first_dataBlock = new dataBlock();
        this->f_size = 0;
        this->N_dataBlocks = 1; // One, for the root inode name-pairs datablock (initially empty datablock, but it exists)

        for(int i = 0; i < 236; i++) {
            this->paddedBlock[i] = '\0'; // Null padding to make the struct 256 bytes
        }
        std::cout << "size inode:" << sizeof(inode) << std::endl;
    }

    /* Constructor */
    inode(int inode_num, int file_size){
        this->first_dataBlock = NULL;
        this->f_size = (int32_t) file_size;
        this->f_inode = (int32_t) inode_num;
        this->N_dataBlocks = 0;

        for(int i = 0; i < 236; i++) {
            this->paddedBlock[i] = '\0'; // Null padding to make the struct 256 bytes
        }
    }
}__attribute__((__packed__)) inode; // Must pack to avoid compiler alignments changing the size of the struct (to maintain 256 bytes)

/**
 * @brief
 * struct of bitMap
*/
typedef struct bitMap{
    bitMap *nextBitMap;
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
    inode *sb_rootnum; /* Pointer to the root block */
    int32_t sb_totalct; /* Total number of files in the file system */
    int32_t sb_freect; /* Number of free data blocks */

    /* Contains the pointer to the bitmap */
    bitMap *bitMapTable; 

    char reserved[231]; /* Reserved bits, to fill up 256 bytes total superblock size */

    /* Constructor, used in initial making of the file system */
    superblock(int numBlocks){
        this->sb_magicnum = TFS_SB_MAGIC_NUM;
        this->sb_rootnum = new inode();
        this->sb_totalct = 1; /* One file in the system currently, the root inode */
        this->sb_freect = numBlocks - 2;    /* Super block takes a space of one block, root inode takes a space of one block */
        this->bitMapTable = new bitMap();

        for(int i = 0; i < 231; i++){
            this->reserved[i] = 0x00;
       }
    }

    /* Default constructor */
    superblock() {
        this->sb_magicnum = 0;
        this->sb_rootnum = NULL;
        this->sb_totalct = 0;
        this->sb_freect = 0;
        this->bitMapTable = NULL;

        for(int i = 0; i < 231; i++){
            this->reserved[i] = 0x00;
       }
    }
}__attribute__((__packed__)) superblock; // Must pack to avoid compiler alignments changing the size of the struct (to maintain 256 bytes)

/**
 * TODO: add more and description and finish destructor
*/
class tfs 
{
    private:
        std::unordered_map<fileDescriptor, inode> openInodes; /* A mapping between open inodes in the file system and their associated file descriptor */
        superblock sb; /* Superblock for the TinyFS file system */

    public:
        fileDescriptor fd = -1; /* Global file descriptor for current disk */

        // Constructor
        tfs()
            : openInodes(std::unordered_map<fileDescriptor, inode>()), 
              sb(superblock()) {}

        // Constructor
        tfs(int numBlocks)
            : openInodes(std::unordered_map<fileDescriptor, inode>()), 
              sb(superblock(numBlocks)) {}

        // Destructor
        ~tfs() 
        {
            delete(this->sb.bitMapTable);
        }

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
