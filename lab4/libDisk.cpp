#include "libDisk.h"
std::vector<int> diskFiles;

int openDisk(char *filename, int nBytes){
    FILE * diskFIle;
    int currFD;
    bool existFD = false;
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

        //get the current files FD
        currFD = fileno(diskFIle);
        if(currFD == -1){
            return OPEN_DISK_ERROR;
        }

        //check if the file discriptor exist in the list
        for(int i = 0; i < (int)diskFiles.size(); i++){
            if(diskFiles[i] == currFD){
                existFD = true;
                break;
            }
        }

        //if file discriptor don't exist add it to the list
        if(existFD == false){
            diskFiles.push_back(currFD);
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

        //get the current files FD
        currFD = fileno(diskFIle);
        if(currFD == -1){
            return OPEN_DISK_ERROR;
        }

        //check if the file discriptor exist in the list
        for(int i = 0; i < (int)diskFiles.size(); i++){
            if(diskFiles[i] == currFD){
                existFD = true;
                break;
            }
        }

        //if file discriptor don't exist add it to the list
        if(existFD == false){
            diskFiles.push_back(currFD);
        }
    }
    
    //returns the recently added fd to access the file
    return currFD;
}


int readBlock(int disk, int bNum, void *block){
    bool found = false;
    //check if the disk fd exist in the diskFiles list
    for(int i = 0; i < (int)diskFiles.size(); i++){
        if(diskFiles[i] == disk){
            found = true;
            break;
        }
    }

    if(!found){
        return DISK_READ_ERROR;
    }

    //if the disk file exist, check the size 
    struct stat fileStat;
    if (fstat(disk, &fileStat) == -1) {
        return DISK_READ_ERROR;
    }

    //check if the offset is greater/equal than the actual file size
    if(fileStat.st_size >= bNum * BLOCKSIZE){
        return DISK_READ_ERROR;
    }

    //perform the seek in the file for offset and read the block for the data
    if (lseek(disk, bNum * BLOCKSIZE, SEEK_SET) == -1) {
        return DISK_READ_ERROR;
    }

    //read the data from the disk based on the BLOCK size
    int bytesRead = read(disk, (char *)block, BLOCKSIZE);
    if(bytesRead == -1){
        return DISK_READ_ERROR;
    }

    //if successfully read the data 
    return DISK_READ_SUCCESS;
}

int writeBlock(int disk, int bNum, void *block){
    return 0;
}

void closeDisk(int disk){
    return;
}