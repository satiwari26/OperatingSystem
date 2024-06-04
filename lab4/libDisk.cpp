#include "libDisk.h"

int openDisk(char *filename, int nBytes){
    if((nBytes % BLOCKSIZE) != 0){
        return OPEN_DISK_ERROR;
    }
    return 0;
}