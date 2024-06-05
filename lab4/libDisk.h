#include <iostream>
#include <stdio.h>
#include <unistd.h>
#define BLOCKSIZE 256

// disk related errors that we have to deal with
#define OPEN_DISK_ERROR -1

/**
 * @brief
 * disk file pointer availaible to be used throughout the project
*/
FILE * diskFIle;

int openDisk(char *filename, int nBytes);