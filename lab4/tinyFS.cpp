#include "tinyFS.h"

using namespace std;

tfs *tinyFS; /* Empty TinyFS */

int main(){
    printf("Hello world!\n");
}

int tfs_mkfs(char *filename, int nBytes)
{
    int numBlocks = nBytes / BLOCKSIZE; /* Maximum number of blocks supported by the tinyFS */

    fileDescriptor fd;

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

    tinyFS = new tfs(numBlocks);

    return SUCCESS_MKFS;
}