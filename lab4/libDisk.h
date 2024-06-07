#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>

#include "macrostfs.h"
#include "errtfs.h"

/**
 * @brief
 * disk file pointer list for all the open DISKS
*/
extern int32_t currFD;


/**
 * @brief
 * opens the disk file and resizes it based on the nBytes provided to it
*/
int32_t openDisk(char *filename, int32_t nBytes);

/**
 * @brief
 * read block data from the disk file
*/
int32_t readBlock(int32_t disk, int32_t bNum, void *block);

/**
 * @brief
 * write block data to the disk file
*/
int32_t writeBlock(int32_t disk, int32_t bNum, void *block);

/**
 * @brief
 * closes the disk file so no read or write can be performed to it
*/
void closeDisk(int32_t disk);