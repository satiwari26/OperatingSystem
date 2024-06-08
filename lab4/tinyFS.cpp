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

fileDescriptor tfs_open(char *name)
{    
    // Check if the file system is mounted (and of the correct type) and that the disk opened successfully
    // tinyFS is null if the mount failed (or has not been called)
    if(tinyFS == NULL || tinyFS->fd < 0) {
        return ERROR_TFS_OPEN;
    }

    int32_t read_result = readBlock(tinyFS->fd, 0, (void*) tinyFS->getSuperblock());
    if (read_result != SUCCESS_READDISK)
    {
        return read_result;
    }

    // Check that the superblock is mounted or not
    // WE TECHNICALLY DO NOT NEED THIS IF STATEMENT. THE TINYFS PTR WILL ONLY BE INSTANTIATED IF THE SYSTEM IS PROPERLY MOUNTED. REDUNDANT CHECK!!
    // TODO: consult the removal of this if statement (or if it really does need to be used aka not redundant)
    if (tinyFS->getSuperblock()->sb_magicnum == TFS_SB_MAGIC_NUM)
    {
        // TODO: Can prob modify filExists to directly return an inode instead of inode num
        int fileInodeNum = tinyFS->fileExists(name);
        inode fileInode;
        cout << "file inode num is " << fileInodeNum << endl;
        // File does not exist, create a new one
        if (fileInodeNum == -1)
        {
            fileInode = tinyFS->createFile(name);
            if (fileInode.f_inode < SUCCESS_TFS_CREATEFILE)
            {
                return ERROR_TFS_CREATEFILE; // Return error stored in the fileInode
            }
        }
        // File does exist, pull its inode data from the disk
        else
        {
            cout << "in here" << endl;
            // if file is already open, return that file descriptor
            for (auto& filePair : tinyFS->openFileStruct)
            {
                if (filePair.second.f_inode == fileInodeNum)
                {
                    return filePair.second.f_inode;
                }
            }

            int fileReadResult = readBlock(tinyFS->fd, fileInodeNum, (void*) &fileInode);
            if (fileReadResult < SUCCESS_READDISK)
            {
                return fileReadResult;
            }
        }

        //update the access time for the current file Inode
        std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
        // Convert the current time point to seconds since the epoch
        std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> currentTimeInSeconds = std::chrono::time_point_cast<std::chrono::seconds>(currentTime);
        // Get the number of seconds since the epoch
        std::chrono::seconds seconds = currentTimeInSeconds.time_since_epoch();
        fileInode.accessTime = seconds.count();

        //update the fileInode by writing it to the disk
        int inodeFlag = tinyFS->writeInodeBlock(fileInode, fileInode.f_inode);
        if(inodeFlag < SUCCESS_WRITEDISK){
            return inodeFlag;
        }

        // Associate a new file descriptor to the file inode and return it to the user
        fileDescriptor tempVirtualFD = tinyFS->getNextVirtualFD();
        tinyFS->setOpenFileStruct(tempVirtualFD, fileInode);  //update the openFileStruct

        return tempVirtualFD;
    }
    else 
    {
        return ERROR_TFS_OPEN;
    }

    return 0;
}

int tfs_write(fileDescriptor FD, char *buffer, int size){
    if(tinyFS == NULL || tinyFS->fd < 0){
        return ERROR_TFS_WRITE;
    }
    //check if the FD is open
    if(tinyFS->openFileStruct.find(FD) == tinyFS->openFileStruct.end()){
        return ERROR_TFS_OPEN;
    }

    //if it is open - find the corresponding inode to it and offset to the right block
    inode writeFileInode = tinyFS->openFileStruct[FD];

    //delete the prev dataBlock associated with this inode
    int delDataBlock = tinyFS->deleteDataBlock(writeFileInode);
    if(delDataBlock < SUCCESS_WRITEDISK){
        return delDataBlock;
    }

    int32_t firstDataBlock = writeFileInode.first_dataBlock;
    //check if there is a data block associated with this file, if not create one
    if(writeFileInode.first_dataBlock == -1){
        firstDataBlock = tinyFS->getNextAvailableInode();
        writeFileInode.first_dataBlock = firstDataBlock;
    }

    //check how many blocks would be needed to write this data with the existing data
    int32_t numBlocksNeeded = (int)std::ceil((float) size / (float) DATABLOCK_MAXSIZE_BYTES);

    int32_t currBlock = writeFileInode.first_dataBlock;

    //if numblocksNeeded is only 1 - size would be less than or equal to 247 bytes
    if(numBlocksNeeded == 1){
        dataBlock dd_block = dataBlock();
        for(int i = 0; i < size; i++){
            memcpy(&dd_block.directDataBlock[i], &buffer[i], sizeof(char));
        }

        //write the data block to the file
        int writeDataCheck = tinyFS->writeDataBlock(dd_block, currBlock);
        if(writeDataCheck < SUCCESS_WRITEDISK){
            return writeDataCheck;
        }
    }
    //if more than 1 blocks needed
    else{
        //create new data block and find space for it
        for(int i = 0; i < numBlocksNeeded; i++){
            int lastSizeBlock = DATABLOCK_MAXSIZE_BYTES;
            dataBlock newDataBlock = dataBlock();


            //size of the last block
            if(i == (numBlocksNeeded - 1)){
                int emptySpace = (numBlocksNeeded * DATABLOCK_MAXSIZE_BYTES) - size;
                lastSizeBlock = DATABLOCK_MAXSIZE_BYTES - emptySpace;
            }

            //write the value to current temp Datablock
            memcpy(&newDataBlock.directDataBlock, &buffer[i * DATABLOCK_MAXSIZE_BYTES], lastSizeBlock);

            int32_t tempCurrBlock = -1;

            // update the next block offset value
            if (i != (numBlocksNeeded - 1)) {
            tinyFS->updateBitMap(currBlock, 1);
            tempCurrBlock = tinyFS->getNextAvailableInode();
            }

            newDataBlock.nextDataBlock = tempCurrBlock;

            int writeDataBlock = tinyFS->writeDataBlock(newDataBlock, currBlock);
            
            if(writeDataBlock < SUCCESS_WRITEDISK){
                return writeDataBlock;
            }

            currBlock = tempCurrBlock;

        }
    }

    writeFileInode.N_dataBlocks = numBlocksNeeded;
    writeFileInode.f_size = size;

    //update the modification time for the current file Inode
    std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
    // Convert the current time point to seconds since the epoch
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> currentTimeInSeconds = std::chrono::time_point_cast<std::chrono::seconds>(currentTime);
    // Get the number of seconds since the epoch
    std::chrono::seconds seconds = currentTimeInSeconds.time_since_epoch();
    writeFileInode.modificationTime = seconds.count();

    int32_t writeInodeResult = tinyFS->writeInodeBlock(writeFileInode, writeFileInode.f_inode);
    if (writeInodeResult < SUCCESS_WRITEDISK)
    {
        return writeInodeResult;
    }
    tinyFS->updateBitMap(writeFileInode.f_inode, 1);
}

int32_t tfs_close(fileDescriptor FD)
{
    if (tinyFS->closeOpenFD(FD) < SUCCESS_TFS_CLOSE)
    {
        return ERROR_TFS_CLOSE;
    }
    return SUCCESS_TFS_CLOSE;
}

int32_t tfs_delete(fileDescriptor FD)
{
    if (tinyFS->openFileStruct.find(FD) == tinyFS->openFileStruct.end())
    {
        return ERROR_TFS_DELETE;
    }    
    
    tinyFS->deleteRootDataEntry(tinyFS->openFileStruct[FD].f_inode);
    tinyFS->deleteFileUpdate(FD);
}

int32_t tfs_readByte(fileDescriptor FD, char *buffer)
{
    // Check if the file system is mounted (and of the correct type) and that the disk opened successfully
    // tinyFS is null if the mount failed (or has not been called)
    if (tinyFS == NULL || tinyFS->fd < 0 || tinyFS->openFileStruct.find(FD) == tinyFS->openFileStruct.end())
    {
        return ERROR_TFS_READBYTE;
    }

    inode inodeToRead; /* The inode to read a byte of data from */

    int inode_read_result = readBlock(tinyFS->fd, tinyFS->openFileStruct[FD].f_inode, (void*) &inodeToRead);
    if (inode_read_result != SUCCESS_READDISK)
    {
        return inode_read_result; // Throw error returned from disk read
    }

    //update the access time on the file inode
    std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
    // Convert the current time point to seconds since the epoch
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> currentTimeInSeconds = std::chrono::time_point_cast<std::chrono::seconds>(currentTime);
    // Get the number of seconds since the epoch
    std::chrono::seconds seconds = currentTimeInSeconds.time_since_epoch();
    inodeToRead.accessTime = seconds.count();

    //update the inode table by writing it to the disk
    int updateInodeData = tinyFS->writeInodeBlock(inodeToRead, inodeToRead.f_inode);
    if(updateInodeData < SUCCESS_WRITEDISK){
        return updateInodeData;
    }

    // Find the corresponding data block through its index and the offset, to the byte, within that data block
    // 247 bytes refers to the max number of entries that can be held per data block
    int32_t blockIndex = inodeToRead.f_offset / DATABLOCK_MAXSIZE_BYTES;
    int32_t blockOffset = inodeToRead.f_offset % DATABLOCK_MAXSIZE_BYTES;
    dataBlock dataBlockTemp;

    if (inodeToRead.f_offset >= inodeToRead.f_size)
    {
        return ERROR_TFS_READBYTE;
    }

    if (blockIndex == 0)
    {
        int data_read_result = readBlock(tinyFS->fd, inodeToRead.first_dataBlock, (void*) &dataBlockTemp);
        if (data_read_result != SUCCESS_READDISK)
        {
            return data_read_result; // Throw error returned from disk read
        }
    }
    else
    {
        // If another data block exists...
        // we need to iterate through the datablocks to get to the correct one, indicated by blockIndex
        for (int32_t i = 0; i < blockIndex; i++)
        {
            if (dataBlockTemp.nextDataBlock != -1)
            {
                dataBlock nextDataBlockTemp;
                
                int data_read_result = readBlock(tinyFS->fd, dataBlockTemp.nextDataBlock, (void*) &nextDataBlockTemp);
                if (data_read_result != SUCCESS_READDISK)
                {
                    return data_read_result; // Throw error returned from disk read
                }
                
                dataBlockTemp = nextDataBlockTemp; 
            }
        }
    }

    // Ensure that the block offset begins at least 1 byte before the last entry in the data block.
    if (blockOffset < DATABLOCK_MAXSIZE_BYTES) 
    {
        memcpy(buffer, &dataBlockTemp.directDataBlock[blockOffset], 1);
        inodeToRead.f_offset+=1;
        tinyFS->writeInodeBlock(inodeToRead, inodeToRead.f_inode);
    
        return SUCCESS_TFS_READBYTE;
    }
    else
    {
        return ERROR_TFS_READBYTE;
    }
}

int tfs_seek(fileDescriptor FD, int offset)
{
    if (tinyFS == NULL || tinyFS->fd < 0 || tinyFS->openFileStruct.find(FD) == tinyFS->openFileStruct.end())
    {
        return ERROR_TFS_SEEK;
    }

    inode inodeToSeek; /* The inode to read a byte of data from */

    int inode_read_result = readBlock(tinyFS->fd, tinyFS->openFileStruct[FD].f_inode, (void*) &inodeToSeek);
    if (inode_read_result != SUCCESS_READDISK)
    {
        return inode_read_result; // Throw error returned from disk read
    }

    // Change the offset, in bytes
    inodeToSeek.f_offset = (int32_t) offset;

    tinyFS->writeInodeBlock(inodeToSeek, inodeToSeek.f_inode);

    return SUCCESS_TFS_SEEK;
}

int32_t tfs_rename(fileDescriptor FD, char* name)
{
    // Check if the file system is mounted (and of the correct type) and that the disk opened successfully
    // tinyFS is null if the mount failed (or has not been called)
    // Also make sure that the file is open
    if (tinyFS == NULL || tinyFS->fd < 0 || tinyFS->openFileStruct.find(FD) == tinyFS->openFileStruct.end())
    {
        return ERROR_TFS_RENAMEFILE;
    }

    // Grab the root node first data block 
    dataBlock rootNodeData;
    int32_t curBlockNum = ROOT_NODE_FIRST_DATA_BLOCK;
    int32_t rootReadResult = readBlock(tinyFS->fd, ROOT_NODE_FIRST_DATA_BLOCK, &rootNodeData);
    if(rootReadResult < SUCCESS_READDISK) {
        return rootReadResult;
    }

    for (int curPairOffset = 0; curPairOffset < DATABLOCK_MAXSIZE_BYTES; curPairOffset+=DATABLOCK_ENTRY_SIZE)
    {
        char curPairFileName[DATABLOCK_FILENAME_SIZE] = { '\0' }; // Initialized to all null terminator
        // If the file is equal, then we need to retrieve the corresponding inode number and check if that block
        // has been allocated on the bitmap (to ensure that it is a stable entry)
        int32_t curPairInodeNum = -1;

        memcpy(curPairFileName, &rootNodeData.directDataBlock[curPairOffset], DATABLOCK_FILENAME_SIZE);
        memcpy(&curPairInodeNum, &rootNodeData.directDataBlock[curPairOffset] + DATABLOCK_FILENAME_SIZE, sizeof(int32_t));

        if (tinyFS->openFileStruct[FD].f_inode == curPairInodeNum)
        {
            // File exists, so rename it
            memcpy(&rootNodeData.directDataBlock[curPairOffset], name, DATABLOCK_FILENAME_SIZE);
            tinyFS->writeDataBlock(rootNodeData, curBlockNum);
            return SUCCESS_TFS_RENAMEFILE;
        }
    }

    // Search all name-value pairs for the file name
    while (rootNodeData.nextDataBlock != -1)
    {
        // Read the next data block, for use in the NEXT iteration of this while loop
        dataBlock nextRootNodeData;

        int32_t nextRootReadResult = readBlock(tinyFS->fd, rootNodeData.nextDataBlock, &nextRootNodeData);
        if(nextRootReadResult < SUCCESS_READDISK) {
            return nextRootReadResult;
        }

        for (int curPairOffset = 0; curPairOffset < DATABLOCK_MAXSIZE_BYTES; curPairOffset+=DATABLOCK_ENTRY_SIZE)
        {
            char curPairFileName[DATABLOCK_FILENAME_SIZE] = { '\0' }; // Initialized to all null terminator
            // If the file is equal, then we need to retrieve the corresponding inode number and check if that block
            // has been allocated on the bitmap (to ensure that it is a stable entry)
            int32_t curPairInodeNum = -1;

            memcpy(curPairFileName, &rootNodeData.directDataBlock[curPairOffset], DATABLOCK_FILENAME_SIZE);
            memcpy(&curPairInodeNum, &rootNodeData.directDataBlock[curPairOffset] + DATABLOCK_FILENAME_SIZE, sizeof(int32_t));

            if (strcmp(name, curPairFileName) == 0 && tinyFS->bit_map.bitmap[curPairInodeNum] == BLOCK_ALLOCATED)
            {
                // File exists, so rename it
                memcpy(&rootNodeData.directDataBlock[curPairOffset], name, DATABLOCK_FILENAME_SIZE);
                tinyFS->writeDataBlock(rootNodeData, curBlockNum);
                return SUCCESS_TFS_RENAMEFILE;
            }
        }
        curBlockNum = rootNodeData.nextDataBlock;
        rootNodeData = nextRootNodeData; // Move onto the next data block to be read
    }
    // Else return -1 as the file does not exist (either it's name was not found in the entries or it was not allocated properly)
    return ERROR_TFS_RENAMEFILE;
}

void tfs_readdir()
{
    // Grab the root node first data block 
    dataBlock rootNodeData;
    int32_t rootReadResult = readBlock(tinyFS->fd, ROOT_NODE_FIRST_DATA_BLOCK, &rootNodeData);
    if(rootReadResult < SUCCESS_READDISK) {
        return;
    }
    
    // Start listing entries from the root dir
    cout << "ROOT DIR" << endl;
    for (int curPairOffset = 0; curPairOffset < DATABLOCK_MAXSIZE_BYTES; curPairOffset+=DATABLOCK_ENTRY_SIZE)
    {
        char curPairFileName[DATABLOCK_FILENAME_SIZE] = { '\0' }; // Initialized to all null terminator
        // If the file is equal, then we need to retrieve the corresponding inode number and check if that block
        // has been allocated on the bitmap (to ensure that it is a stable entry)
        int32_t curPairInodeNum = -1;

        memcpy(curPairFileName, &rootNodeData.directDataBlock[curPairOffset], DATABLOCK_FILENAME_SIZE);
        memcpy(&curPairInodeNum, &rootNodeData.directDataBlock[curPairOffset] + DATABLOCK_FILENAME_SIZE, sizeof(int32_t));

        if (tinyFS->bit_map.bitmap[curPairInodeNum] == BLOCK_ALLOCATED)
        {
            cout << "- " << curPairFileName << endl; 
        }
    }

    // Search all name-value pairs for the file name
    while (rootNodeData.nextDataBlock != -1)
    {
        // Read the next data block, for use in the NEXT iteration of this while loop
        dataBlock nextRootNodeData;
        int32_t nextRootReadResult = readBlock(tinyFS->fd, rootNodeData.nextDataBlock, &nextRootNodeData);
        if(nextRootReadResult < SUCCESS_READDISK) {
            return;
        }

        // Start listing entries from the root dir
        cout << "ROOT DIR" << endl;
        for (int curPairOffset = 0; curPairOffset < DATABLOCK_MAXSIZE_BYTES; curPairOffset+=DATABLOCK_ENTRY_SIZE)
        {
            char curPairFileName[DATABLOCK_FILENAME_SIZE] = { '\0' }; // Initialized to all null terminator
            // If the file is equal, then we need to retrieve the corresponding inode number and check if that block
            // has been allocated on the bitmap (to ensure that it is a stable entry)
            int32_t curPairInodeNum = -1;

            memcpy(curPairFileName, &rootNodeData.directDataBlock[curPairOffset], DATABLOCK_FILENAME_SIZE);
            memcpy(&curPairInodeNum, &rootNodeData.directDataBlock[curPairOffset] + DATABLOCK_FILENAME_SIZE, sizeof(int32_t));

            if (tinyFS->bit_map.bitmap[curPairInodeNum] == BLOCK_ALLOCATED)
            {
                cout << "- " << curPairFileName << endl; 
            }
        }
        rootNodeData = nextRootNodeData; // Move onto the next data block to be read
    }
}

inode tfs_stat(fileDescriptor FD)
{
    // Check if the file system is mounted (and of the correct type) and that the disk opened successfully
    // tinyFS is null if the mount failed (or has not been called)
    // Also make sure that the file is open
    if (tinyFS == NULL || tinyFS->fd < 0 || tinyFS->openFileStruct.find(FD) == tinyFS->openFileStruct.end())
    {
        return ERROR_TFS_STAT;
    }

    inode inodeToStat; /* The inode to stat */

    int inode_read_result = readBlock(tinyFS->fd, tinyFS->openFileStruct[FD].f_inode, (void*) &inodeToStat);
    if (inode_read_result != SUCCESS_READDISK)
    {
        return inode(false); // Throw "null" inode to the user
    }

    return inodeToStat;
}