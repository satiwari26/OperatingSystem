#include "tinyFS.h"

using namespace std;

tfs *tinyFS; /* Empty TinyFS */

int tfs_mkfs(char *filename, int nBytes)
{
    int numBlocks = nBytes / BLOCKSIZE; /* Maximum number of blocks supported by the tinyFS */

    fileDescriptor fd = -1; /* Local file descriptor for formatting the file system */

    if (filename == NULL || strlen(filename) > MAX_FILENAME_LEN)
    {
        filename = (char*) (const char*) DEFAULT_DISK_NAME;
    }

    if ((fd = openDisk(filename, nBytes)) < SUCCESS_OPENDISK)
    {
        cerr << "Error: could not open file from disk : " << fd << endl; 
        closeDisk(fd);
        return fd; // Return the error code stored in the file descriptor
    }

    // Initialize all the data to 0x00
    char zero_bytes[BLOCKSIZE] = { 0 };
    for (int curBlock = 0; curBlock < numBlocks; curBlock++)
    {
        int write_result = writeBlock((int) fd,  curBlock, zero_bytes);

        // Return error code from the result, if the system did not successfully write a block to disk
        if (write_result != SUCCESS_WRITEDISK)
        {
            closeDisk(fd);
            return write_result; 
        }
    }
    
    // Write the superblock to the disk
    superblock sb = superblock(numBlocks);
    
    int sb_result = writeBlock((int) fd, SUPERBLOCK_NUM, (void*) &sb);
    cout << "size sb:" << sizeof(sb) << endl;
    // Return error code from the result, if the system did not successfully write a block to disk
    if (sb_result != SUCCESS_WRITEDISK)
    {
        closeDisk(fd);
        return sb_result; 
    }

    closeDisk((int) fd);

    return SUCCESS_TFS_MKFS;
}

int tfs_mount(char* filename)
{
    tinyFS = new tfs();

    // Open the disk and read the superblock from it
    tinyFS->fd = openDisk(filename, 0);
    int read_result = readBlock(tinyFS->fd, 0, (void*) tinyFS->getSuperblock());
    if (read_result != SUCCESS_READDISK)
    {
        delete(tinyFS);
        return read_result;
    }

    // Check that the superblock magic number matches the TinyFS magic number
    if (tinyFS->getSuperblock()->sb_magicnum == TFS_SB_MAGIC_NUM)
    {
        return SUCCESS_TFS_MOUNT;
    }
    else 
    {
        delete(tinyFS);
        return ERROR_TFS_MOUNT;
    }
}

int tfs_unmount(void)
{
    if (tinyFS != NULL)
    {
        closeDisk(tinyFS->fd);
        delete(tinyFS);

        return 0;
    }
    return ERROR_TFS_UNMOUNT;
}