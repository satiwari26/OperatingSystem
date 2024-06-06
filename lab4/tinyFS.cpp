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
        int result = writeBlock((int) fd,  curBlock, zero_bytes);

        // Return error code from the result, if the system did not successfully write a block to disk
        if (result < DISK_WRITE_SUCCESS)
        {
            return result; 
        }
    }

    tinyFS = new tfs(numBlocks);

    // Write the superblock to the disk
    int sb_result = writeBlock((int) fd, SUPERBLOCK_NUM, (void*) tinyFS->getSuperblock());
    cout << "size sb:" << sizeof(tinyFS->getSuperblock()) << endl;
    // Return error code from the result, if the system did not successfully write a block to disk
    if (sb_result < DISK_WRITE_SUCCESS)
    {
        return sb_result; 
    }

    // Initalize the inode list
    vector<inode> inodeList;
    inodeList.reserve(INODE_COUNT);
    for (int i = 0; i < INODE_COUNT; i++)
    {   
        inodeList.emplace_back();
        cout << "size inode num" << i << ": " << sizeof(inodeList[i]) << endl;
    }
    
    // Write inode list to the disk
    for (int i = 0; i < INODE_COUNT; i++)
    {
        int il_result = writeBlock((int) fd, i, inodeList.data());
        
        if (il_result < DISK_WRITE_SUCCESS)
        {
            return il_result; 
        }
    }
   
    return SUCCESS_MKFS;
}