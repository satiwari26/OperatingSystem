#include "tinyFS.h"

using namespace std;

tfs *tinyFS; /* Empty TinyFS */

int32_t tfs_mkfs(char *filename, int32_t nBytes)
{
    int32_t numBlocks = nBytes / BLOCKSIZE; /* Maximum number of blocks supported by the tinyFS */

    tfs tfsNew = tfs(numBlocks); /* New file system to be formatted to the disk */

    if (filename == NULL || strlen(filename) > MAX_FILENAME_LEN)
    {
        filename = (char*) (const char*) DEFAULT_DISK_NAME;
    }

    if ((tfsNew.fd = openDisk(filename, nBytes)) < SUCCESS_OPENDISK)
    {
        cerr << "Error: could not open file from disk : " << tfsNew.fd << endl; 
        closeDisk(tfsNew.fd);
        return tfsNew.fd; // Return the error code stored in the file descriptor
    }

    // Initialize all the data to 0x00
    char zero_bytes[BLOCKSIZE] = { 0 };
    for (int32_t curBlock = 0; curBlock < numBlocks; curBlock++)
    {
        int32_t write_result = writeBlock((int) tfsNew.fd,  curBlock, zero_bytes);

        // Return error code from the result, if the system did not successfully write a block to disk
        if (write_result != SUCCESS_WRITEDISK)
        {
            closeDisk(tfsNew.fd);
            return write_result; 
        }
    }
    
    // // Write the superblock to the disk
    int32_t sb_result = writeBlock((int) tfsNew.fd, SUPERBLOCK_NUM, (void*) tfsNew.getSuperblock());
    cout << "size sb:" << sizeof(*tfsNew.getSuperblock()) << endl;
    // Return error code from the result, if the system did not successfully write a superblock to disk
    if (sb_result != SUCCESS_WRITEDISK)
    {
        closeDisk(tfsNew.fd);
        return sb_result; 
    }

    // Write the root inode to the disk
    inode rootInode = inode();
    int32_t root_result = writeBlock((int) tfsNew.fd, ROOT_NODE_BLOCK_NUM, (void*) &rootInode);
    cout << "size root inode:" << sizeof(rootInode) << endl;
    // Return error code from the result, if the system did not successfulyl write a root inode block to the disk
    if (root_result != SUCCESS_WRITEDISK)
    {
        closeDisk(tfsNew.fd);
        return root_result; 
    }

    // Write the bitmap to the disk
    bitMap initBitMap = bitMap();
    int32_t bitmap_result = writeBlock((int) tfsNew.fd, BITMAP_BLOCK_NUM, (void*) &initBitMap);
    cout << "size bitmap:" << sizeof(initBitMap) << endl;
    // Return error code from the result, if the system did not successfulyl write a bitmap block to the disk
    if (bitmap_result != SUCCESS_WRITEDISK)
    {
        closeDisk(tfsNew.fd);
        return bitmap_result; 
    }

    // No need to write the root inode's data block (name-value pairs) to the disk.
    // It is initially empty, and it's associated block number is already defined in the superblock.
    // It is also allocated in the bitmap.

    closeDisk((int) tfsNew.fd);

    return SUCCESS_TFS_MKFS;
}

int32_t tfs_mount(char* filename)
{
    tinyFS = new tfs();

    // Open the disk and store the file descriptor in the TFS
    tinyFS->fd = openDisk(filename, 0);
    if (tinyFS->fd < SUCCESS_OPENDISK)
    {
        delete(tinyFS);
        return tinyFS->fd; // Return error from opening disk, stored in fd result
    }
    
    // Read the superblock from the disk
    int read_result = readBlock(tinyFS->fd, SUPERBLOCK_NUM, (void*) tinyFS->getSuperblock());
    if (read_result != SUCCESS_READDISK)
    {
        delete(tinyFS);
        return read_result; // Return error from reading disk, stored in read_result
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

int32_t tfs_unmount(void)
{
    if (tinyFS != NULL)
    {
        int diskToClose = tinyFS->fd;
        delete(tinyFS);
        closeDisk(diskToClose);
        
        return 0;
    }
    return ERROR_TFS_UNMOUNT;
}

fileDescriptor tfs_open(char *name){
    //check if the root inode exist
    if(tinyFS->fd < 0){
        return ERROR_TFS_OPEN;
    }
    int32_t read_result = readBlock(tinyFS->fd, 0, (void*) tinyFS->getSuperblock());
    if (read_result != SUCCESS_READDISK)
    {
        delete(tinyFS);
        return read_result;
    }

    // Check that the superblock is mounted or not
    if (tinyFS->getSuperblock()->sb_magicnum == TFS_SB_MAGIC_NUM)
    {
        //find the inode and corresponding FD for this file
        int32_t tempInodeNumber = tinyFS->getNextAvailableInode();
        fileDescriptor tempVirtualFD = tinyFS->getNextVirtualFD();

        //create a new InodeBlock add it the file, update the bitMap, add the FD to openFileStruct, create datablock, update the root node with name-inode value pair
        inode tempNode = inode(tempInodeNumber);
        int32_t updateBitMapReturnValue = tinyFS->updateBitMap(tempInodeNumber, 0);  //update the bitmap for the corresponding
        if(updateBitMapReturnValue < SUCCESS_WRITEDISK){
            return updateBitMapReturnValue;
        }
        tinyFS->setOpenFileStruct(tempVirtualFD, tempInodeNumber);  //update the openFileStruct





        /*we need to chage the inode next data block from pointer to offset number*/


    }
    else 
    {
        delete(tinyFS);
        return ERROR_TFS_MOUNT;
    }


    //create root inode, write to disk
    inode rootInode;

    //create dataBlock, write dataStruct to it, write to disk

    //update the openFileStruct
}