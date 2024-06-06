#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>

#define BLOCKSIZE 256
#ifndef DEFAULT_DISK_SIZE
#define DEFAULT_DISK_SIZE 10240  /* Default disk size, in bytes */
#endif



/**
 * libDisk success/error macros
*/
/** SUCCESS CODES BEGIN **/
#ifndef SUCCESS_OPENDISK
#define SUCCESS_OPENDISK 0  /* Successfully opened the disk file */ 
#endif
#ifndef SUCCESS_READDISK
#define SUCCESS_READDISK 0  /* Successfully read from disk file */ 
#endif
#ifndef SUCCESS_WRITEDISK
#define SUCCESS_WRITEDISK 0  /* Successfully wrote to disk file */ 
#endif
/** SUCCESS CODES END **/

/** ERROR CODES BEGIN **/
/* Open disk-related errors*/
#ifndef ERROR_DISK_BLKSIZE
#define ERROR_DISK_BLKSIZE -2000 /* Error due to disk size not being perfectly divisible by the block size */
#endif
#ifndef ERROR_DISK_OPEN
#define ERROR_DISK_OPEN -2001   /* Error opening the disk file */
#endif
#ifndef ERROR_DISK_FD
#define ERROR_DISK_FD -2002     /* Error retrieving the file descriptor for the disk file */
#endif
#ifndef ERROR_DISK_TRUNCATE
#define ERROR_DISK_TRUNCATE -2003    /* Error truncating the disk file */
#endif
#ifndef ERROR_DISK_ACCESS
#define ERROR_DISK_ACCESS -2004    /* Error accessing the disk file, it may not exist in the current directory */
#endif

/* Read/write disk-related errors */
#ifndef ERROR_DISK_FSTAT
#define ERROR_DISK_FSTAT -2005    /* Error accessing the disk file, it may not exist in the current directory */
#endif
#ifndef ERROR_DISK_FOFFSET
#define ERROR_DISK_FOFFSET -2006    /* Error when offset, into the disk, is greater than or equal to the actual file size */
#endif
#ifndef ERROR_DISK_LSEEK
#define ERROR_DISK_LSEEK -2007    /* Error seeking, from an offset, through the disk file */
#endif
#ifndef ERROR_DISK_READ
#define ERROR_DISK_READ -2008    /* Error reading from the disk file */
#endif
#ifndef ERROR_DISK_WRITE
#define ERROR_DISK_WRITE -2009    /* Error writing to the disk file */
#endif
/** ERROR CODES END **/

/**
 * @brief
 * disk file pointer list for all the open DISKS
*/
extern int currFD;


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