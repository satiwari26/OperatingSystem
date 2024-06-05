#include "libDisk.h"

int openDisk(char *filename, int nBytes){
    // if nbytes not perfectly divisible by the block size
    if((nBytes % BLOCKSIZE) != 0){
        return OPEN_DISK_ERROR;
    }

    //if nBytes > 0 open the existing file or new file and resize it
    diskFIle = NULL;
    if(nBytes > 0){
        diskFIle = fopen(filename, "r+");
        //opening the file error
        if(diskFIle == NULL){
            return OPEN_DISK_ERROR;
        }

        //resize the file to the given nBytes
        if (ftruncate(fileno(diskFIle), nBytes) != 0) {
            return OPEN_DISK_ERROR;
        }
    }
    // if the nbytes is 0
    else if(nBytes == 0){
        //check if the file exist in the directory
        if(access(filename, F_OK) == -1){
            return OPEN_DISK_ERROR;
        }
        
        // if file does exist open it without resizing it
        diskFIle = fopen(filename, "r+");
        //opening the file error
        if(diskFIle == NULL){
            return OPEN_DISK_ERROR;
        }
    }



    return 0;
}