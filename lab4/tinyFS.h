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
#ifndef ERROR_TFS_OPEN
#define ERROR_TFS_OPEN -3000
#endif

/*****
 *      TEST VARIABLES/MACROS FOR TFS. WORK IN PROGRESS.
 *****/
#define INODE_COUNT 16
#define SUPERBLOCK_NUM 0
#define BITMAP_BLOCK_NUM 2
#define ROOT_NODE_BLOCK_NUM 1
#define ROOT_NODE_FIRST_DATA_BLOCK 3
#define ROOT_INODE_NUM 2
#define MAX_FILENAME_LEN 8
/* END OF TEST VARIABLES/MACROS */

typedef int fileDescriptor;

/** @brief
 * struct of dataBlock
 * 
 * 240 is even divisible by 12 (8 bytes for name, 4 for inode num)
*/
typedef struct dataBlock {
    int32_t nextDataBlock;
    char directDataBlock[240];
    char paddedBlock[8];

    /* Default constructor */
    dataBlock() {
        this->nextDataBlock = -1;   //initialize it to -1

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
 * 
 * 
 * Inode number is mapped 1:1 with the block number,
 * the next free block is the inode number
*/
typedef struct inode
{
    int32_t f_inode; /* File inode number */
    int32_t first_dataBlock; // pointer to the data block of the current file
    int32_t f_size; // contains the data size of the file
    int32_t N_dataBlocks;   // contains the number of datablock information
    char paddedBlock[232];

    /* Default constructor, typically for root inode */
    inode(){
        this->f_inode = ROOT_INODE_NUM; /* Root inode is stored at number 2 */
        this->first_dataBlock = ROOT_NODE_FIRST_DATA_BLOCK;
        this->f_size = 0;
        this->N_dataBlocks = 1; // One, for the root inode name-pairs datablock (initially empty datablock, but it exists)

        for(int i = 0; i < 232; i++) {
            this->paddedBlock[i] = '\0'; // Null padding to make the struct 256 bytes
        }
        std::cout << "size inode:" << sizeof(inode) << std::endl;
    }

    /* Constructor */
    inode(int inode_num, int file_size){
        this->first_dataBlock = -1;
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
 * 
 * limitation if the disk size is greater than TFS_SB_MAPSIZE our file system won't be able to utlize that extra space
 * since it is only limited to one block i.e 256 bytes
*/
typedef struct bitMap{
    std::bitset<TFS_SB_MAPSIZE> bitmap;

    /* Default constructor */
    bitMap()
    {
        for (int i = 0; i < TFS_SB_MAPSIZE; i++)
        {
            bitmap[i] = 0;
        }

        //mark them in use
        bitmap[SUPERBLOCK_NUM] = 1;
        bitmap[ROOT_NODE_BLOCK_NUM] = 1;
        bitmap[BITMAP_BLOCK_NUM] = 1;
    }
}bitMap;

/**
 * A block that stores metadata about the file system. It is always stored at 
 * logical block 0 on the disk. 
*/
typedef struct superblock
{
    int8_t sb_magicnum; /* The magic number, 0x5A, for TinyFS */
    int32_t sb_rootnum; /* Pointer to the root block */
    int32_t sb_totalct; /* Total number of files in the file system */
    int32_t sb_freect; /* Number of free data blocks */

    /* Contains the pointer to the bitmap */
    int32_t bitMapTable; 

    char reserved[231]; /* Reserved bits, to fill up 256 bytes total superblock size */

    /* Constructor, used in initial making of the file system */
    superblock(int numBlocks){
        this->sb_magicnum = TFS_SB_MAGIC_NUM;
        this->sb_rootnum = ROOT_NODE_BLOCK_NUM;
        this->sb_totalct = 1; /* One file in the system currently, the root inode */
        this->sb_freect = numBlocks - 3;    /* Super block takes a space of one block, root inode takes a space of one block and bitmap block */
        this->bitMapTable = BITMAP_BLOCK_NUM;

        for(int i = 0; i < 231; i++){
            this->reserved[i] = 0x00;
       }
    }

    /* Default constructor */
    superblock() {
        this->sb_magicnum = 0;
        this->sb_rootnum = -1;
        this->sb_totalct = 0;
        this->sb_freect = 0;
        this->bitMapTable = -1; //initially -1

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
        // std::unordered_map<std:: string, inode> rootNodeDataStruct; /* A mapping between open inodes in the file system and their associated file name */
        superblock sb; /* Superblock for the TinyFS file system */

    public:
        int totalNumBlocksOnDisk;
        fileDescriptor fd = -1; /* Global file descriptor for current disk */
        fileDescriptor virtualFD = 2; /*virtual FD for to keep track of the open virtual files*/
        vector<fileDescriptor> freeFileDescriptors; //keep track of the FD that are avaialible to use next
        std::unordered_map<fileDescriptor , inode> openFileStruct; /* A mapping between the inodes and fileDescriptor maintained by tfs_open() and tfs_close()*/
        bitMap bit_map; //keeps track of the bitmap struct 
        // Constructor
        tfs() :sb(superblock()), virtualFD(2) {}    /*initial virtual FD to 2*/

        // Constructor
        tfs(int numBlocks) :sb(superblock(numBlocks)), virtualFD(2), bit_map(bitMap()) {}  /*initial virtual FD to 2*/
              

        // Destructor
        ~tfs() 
        {
            
        }

        void setOpenFileStruct(const fileDescriptor key, const inode value){
            openFileStruct[key] = value;
        }

        superblock* getSuperblock()
        {
            return &this->sb;
        }

        int getNextAvailableInode(){
            int tempInode;
            for(int i = 0; i < totalNumBlocksOnDisk; i++){
                if(this->bit_map.bitmap[i] == 0){
                    tempInode = i;
                    break;
                }
            }
            return tempInode;
        }
        int getNextVirtualFD(){
            int tempFD = this->freeFileDescriptors[0];
            this->freeFileDescriptors.erase(this->freeFileDescriptors.begin());
            return tempFD;
        }
        //close the open FD and adds it to the free list
        void closeOpenFD(fileDescriptor key){
            this->freeFileDescriptors.push_back(key);
            this->openFileStruct.erase(key);
        }
        //update the bitmap
        int updateBitMap(int offset, int value){
            this->bit_map.bitmap[offset] = value;

            //write the block to the disk
            int bitsWrite = writeBlock(this->fd, BITMAP_BLOCK_NUM, (void *)&this->bit_map);
            return bitsWrite;
        }
        //deleting the file and updating struct
        int deleteFileUpdate(fileDescriptor fd){
            //store tje inode temp
            inode tempInode = openFileStruct[fd];

            //if file is open close it first
            if(this->openFileStruct.find(fd) != this->openFileStruct.end()){
                closeOpenFD(fd);
            }

            
            //update the bitmap table by removing the data block for corresponding Inode
            int nextBlockNum = tempInode.first_dataBlock;
            for(int i=0; i<tempInode.N_dataBlocks; i++){
                //update the bit map for next data block num
                updateBitMap(nextBlockNum, 0);

                dataBlock tempDataBlock = dataBlock();
                int bitsRead = readBlock(this->fd, nextBlockNum, (void*) &tempDataBlock);
                if(bitsRead < SUCCESS_READDISK){
                    return bitsRead;
                }
                nextBlockNum = tempDataBlock.nextDataBlock;
            }

            //update the inode table itself
            int bitsUpdate = updateBitMap(tempInode.f_inode, 0);
            if(bitsUpdate < SUCCESS_WRITEDISK){
                return bitsUpdate;
            }
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
