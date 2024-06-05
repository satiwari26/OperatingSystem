#include "tinyFS.h"

using namespace std;

int main(){
    printf("Hello world!\n");
}

int tfs_mkfs(char *filename, int nBytes)
{
    fileDescriptor fd;
    vector<bool> bitMap(256, false); 
    int numBlocks = nBytes / BLOCKSIZE; /* Maximum number of blocks supported by the tinyFS */

    if ((fd = openDisk(filename, nBytes)) < SUCCESS_OPENDISK)
    {
        cerr << "Error: could not open file from disk : " << fd << endl; 
        return fd; // Return the error code stored in the file descriptor
    }

    // Initialize all the data to 0x00
    char zero_bytes[BLOCKSIZE] = { 0 };
    for (int curBlock = 0; curBlock < numBlocks; curBlock++)
    {
        writeBlock((int) fd,  curBlock, zero_bytes);
    }

    // Superblock in the initial unmounted TFS state
    superblock sb = { TFS_SB_MAGIC_NUM, TFS_SB_ROOTNUM_INIT, TFS_SB_TOTALCT_INIT, 
                        numBlocks, TFS_SB_EMPTY_BITMAP(numBlocks)};
    return SUCCESS_MKFS;
}