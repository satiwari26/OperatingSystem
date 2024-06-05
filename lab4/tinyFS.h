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
#define TFS_SB_ROOTNUM_INIT 0
#endif
#ifndef TFS_SB_TOTALCT_INIT
#define TFS_SB_TOTALCT_INIT 0
#endif
#ifndef TFS_SB_EMPTY_BITMAP
#define TFS_SB_EMPTY_BITMAP(size) std::vector<bool>(size, false) /* An empty bit-map for the initial state of the TFS superblock */
#endif

/**
 * TinyFS success/error code macros
*/
#ifndef SUCCESS_OPENDISK
#define SUCCESS_OPENDISK 0  /* Successfully opened disk */      // TODO: move to libDisk.h
#endif
#ifndef SUCCESS_MKFS
#define SUCCESS_MKFS 0 /* Successfully made a file system */
#endif
#ifndef ERROR_MKFS
#define ERROR_MKFS -1000 /* Failed to make a file system */
#endif

typedef int fileDescriptor;

/**
 * An inode block to keep track of metadata for each file within TinyFS.
*/
typedef struct inode
{
    int32_t f_inode; /* File inode number */
    int16_t f_type; /* File type */
    int32_t f_size; /* File size */
    int32_t f_flags; /* File flags */
    int32_t f_offset; /* Offset where data blocks are stored on disc */
    int64_t f_blksize; /* Number of blocks, in 256-byte chunks, allocated to this file */
}inode;

/**
 * A block that stores metadata about the file system. It is always stored at 
 * logical block 0 on the disk. 
*/
typedef struct superblock
{
    int8_t sb_magicnum; /* The magic number, 0x5A, for TinyFS */
    int32_t sb_rootnum; /* Block number for the root directory inode */
    int32_t sb_totalct; /* Total number of files in the file system */
    int32_t sb_freect; /* Number of free data blocks */
    std::vector<bool> sb_bit_map; /* Bit map for the free blocks */
    
}superblock;

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
