#include "libDisk.h"
int32_t currFD;

int32_t openDisk(char *filename, int32_t nBytes){
    FILE * diskFIle;
    bool existFD = false;
    // if nbytes not perfectly divisible by the block size
    if((nBytes % BLOCKSIZE) != 0 || ((nBytes / BLOCKSIZE) <= 3 && nBytes != 0)){
        return ERROR_DISK_BLKSIZE;
    }

    //if nBytes > 0 open the existing file or new file and resize it
    diskFIle = NULL;
    if(nBytes > 0){
        diskFIle = fopen(filename, "r+");
        //opening the file error
        if(diskFIle == NULL){
            return ERROR_DISK_OPEN;
        }

        //get the current files FD
        currFD = fileno(diskFIle);
        if(currFD == -1){
            return ERROR_DISK_FD;
        }

        //resize the file to the given nBytes
        if (ftruncate(fileno(diskFIle), nBytes) != 0) {
            return ERROR_DISK_TRUNCATE;
        }
    }
    // if the nbytes is 0
    else if(nBytes == 0){
        //check if the file exist in the directory
        if(access(filename, F_OK) == -1){
            return ERROR_DISK_ACCESS;
        }

        // if file does exist open it without resizing it
        diskFIle = fopen(filename, "r+");
        //opening the file error
        if(diskFIle == NULL){
            return ERROR_DISK_OPEN;
        }

        //get the current files FD
        currFD = fileno(diskFIle);
        if(currFD == -1){
            return ERROR_DISK_FD;
        }
    }
    
    //returns the recently added fd to access the file
    return currFD;
}


int32_t readBlock(int32_t disk, int32_t bNum, void *block){
    //if the disk file exist, check the size 
    struct stat fileStat;
    if (fstat(disk, &fileStat) == -1) {
        return ERROR_DISK_FSTAT;
    }

    //check if the offset is greater/equal than the actual file size
    if(fileStat.st_size <= bNum * BLOCKSIZE){
        return ERROR_DISK_FOFFSET;
    }

    //perform the seek in the file for offset and read the block for the data
    if (lseek(disk, bNum * BLOCKSIZE, SEEK_SET) == -1) {
        return ERROR_DISK_LSEEK;
    }

    //read the data from the disk based on the BLOCK size
    int32_t bytesRead = read(disk, (char *)block, BLOCKSIZE);
    if(bytesRead == -1){
        return ERROR_DISK_READ;
    }

    //if successfully read the data 
    return SUCCESS_READDISK;
}

int32_t writeBlock(int32_t disk, int32_t bNum, void *block){
    //if the disk file exist, check the size 
    struct stat fileStat;
    if (fstat(disk, &fileStat) == -1) {
        return ERROR_DISK_FSTAT;
    }

    //check if the offset is greater/equal than the actual file size
    if(fileStat.st_size <= bNum * BLOCKSIZE){
        return ERROR_DISK_FOFFSET;
    }

    //perform the seek in the file for offset and read the block for the data
    if (lseek(disk, bNum * BLOCKSIZE, SEEK_SET) == -1) {
        return ERROR_DISK_LSEEK;
    }

    //read the data from the disk based on the BLOCK size
    int32_t bytesRead = write(disk, (char *)block, BLOCKSIZE);
    if(bytesRead == -1){
        return ERROR_DISK_WRITE;
    }

    //if successfully write the data 
    return SUCCESS_WRITEDISK;
}

void closeDisk(int32_t disk){
    //close the file using file descriptor
    if(close(disk) == -1){
        return;
    }
    return;
}