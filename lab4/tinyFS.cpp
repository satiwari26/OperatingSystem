#include "tinyFS.h"

using namespace std;

tfs *tinyFS; /* Empty TinyFS */

int main()
{
    printf("Hello world!\n");
}

int tfs_mkfs(char *filename, int nBytes)
{
    int numBlocks = nBytes / BLOCKSIZE; /* Maximum number of blocks supported by the tinyFS */

    // Make a new file system
    tinyFS = new tfs(numBlocks);

    if ((tinyFS->fd = openDisk(filename, nBytes)) < SUCCESS_OPENDISK)
    {
        cerr << "Error: could not open file from disk : " << tinyFS->fd << endl; 
        return tinyFS->fd; // Return the error code stored in the file descriptor
    }

    // Initialize all the data to 0x00
    char zero_bytes[BLOCKSIZE] = { 0 };
    for (int curBlock = 0; curBlock < numBlocks; curBlock++)
    {
        int result = writeBlock((int) tinyFS->fd,  curBlock, zero_bytes);

        // Return error code from the result, if the system did not successfully write a block to disk
        if (result < DISK_WRITE_SUCCESS)
        {
            return result; 
        }
    }

    // Write the superblock to the disk
    int sb_result = writeBlock((int) tinyFS->fd, SUPERBLOCK_NUM, (void*) tinyFS->getSuperblock());
    
    // Return error code from the result, if the system did not successfully write a block to disk
    if (sb_result < DISK_WRITE_SUCCESS)
    {
        return sb_result; 
    }

    return SUCCESS_MKFS;
}

int tfs_mount(char* filename)
{
    if (tinyFS != NULL)
    {
   
    
    }
    else return -1; // make this into error macro
}