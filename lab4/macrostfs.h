/* Use this name for a default disk file name */
#ifndef DEFAULT_DISK_NAME
#define DEFAULT_DISK_NAME "tinyFSDisk"
#endif
#ifndef BLOCKSIZE
#define BLOCKSIZE 256       /* Number of bytes per disk block */
#endif
#ifndef DEFAULT_DISK_SIZE
#define DEFAULT_DISK_SIZE 10240  /* Default disk size, in bytes */
#endif

/************************************************
 * libDisk success macros                       *
 ************************************************/
/** SUCCESS CODES BEGIN **/
#ifndef SUCCESS_OPENDISK
#define SUCCESS_OPENDISK 0  /* Successfully opened the disk file */ 
#endif
#ifndef SUCCESS_READDISK
#define SUCCESS_READDISK 0  /* Successfully read from disk file */ 
#endif
#ifndef SUCCESS_WRITEDISK
#define SUCCESS_WRITEDISK 0  /* Successfully wrote to disk file */ 
#endif
/** SUCCESS CODES END **/


/************************************************
 * TinyFS superblock macros                     *
 ************************************************/
#ifndef TFS_SB_MAGIC_NUM
#define TFS_SB_MAGIC_NUM 0x5A /* The magic number for the TinyFS */
#endif
#ifndef TFS_SB_ROOTNUM_INIT
#define TFS_SB_ROOTNUM_INIT 0 /* The initial root number for the TinyFS superblock */
#endif
#ifndef TFS_SB_TOTALCT_INIT
#define TFS_SB_TOTALCT_INIT 0 /* The initial total file count for the TinyFS superblock */
#endif
#ifndef TFS_SB_MAPSIZE
#define TFS_SB_MAPSIZE (256 * 8) /* The bit-map size, in bits, in the TinyFS superblock. It uses up the rest of the space in the superblock struct. */
#endif

/**
 * TinyFS success code macros
*/
#ifndef SUCCESS_TFS_MKFS
#define SUCCESS_TFS_MKFS 0 /* Successfully made a file system */
#endif
#ifndef SUCCESS_TFS_MOUNT
#define SUCCESS_TFS_MOUNT 0 /* Successfully mounted a file system */
#endif
#ifndef SUCCESS_TFS_READBYTE
#define SUCCESS_TFS_READBYTE 0 /* Successfully read a byte of data from a file system */
#endif


/*********************************************************
 *      TEST VARIABLES/MACROS FOR TFS. WORK IN PROGRESS. *
 *********************************************************/
#define INODE_COUNT 16
#define SUPERBLOCK_NUM 0
#define BITMAP_BLOCK_NUM 2
#define ROOT_NODE_BLOCK_NUM 1
#define ROOT_NODE_FIRST_DATA_BLOCK 3
#define MAX_FILENAME_LEN 8
#define DATABLOCK_MAX_ENTRIES 19
#define DATABLOCK_ENTRY_SIZE 13
#define DATABLOCK_MAXSIZE_BYTES (DATABLOCK_MAX_ENTRIES * DATABLOCK_ENTRY_SIZE)

/* END OF TEST VARIABLES/MACROS */

