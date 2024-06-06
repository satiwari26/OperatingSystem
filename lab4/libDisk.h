#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <sys/stat.h> 
#define BLOCKSIZE 256

// disk open related flags
#ifndef SUCCESS_OPENDISK
#define SUCCESS_OPENDISK 0  /* Successfully opened disk */      // TODO: move to libDisk.h
#endif
#define OPEN_DISK_ERROR -1

//disk read related flag
#define DISK_READ_ERROR -1
#define DISK_READ_SUCCESS 0
//disk write related flag
#define DISK_WRITE_ERROR -1
#define DISK_WRITE_SUCCESS 0


/**
 * @brief
 * disk file pointer list for all the open DISKS
*/
extern std::vector<int> diskFiles;


/**
 * @brief
 * opens the disk file and resizes it based on the nBytes provided to it
*/
int openDisk(char *filename, int nBytes);

/**
 * @brief
 * read block data from the disk file
*/
int readBlock(int disk, int bNum, void *block);

/**
 * @brief
 * write block data to the disk file
*/
int writeBlock(int disk, int bNum, void *block);

/**
 * @brief
 * closes the disk file so no read or write can be performed to it
*/
void closeDisk(int disk);