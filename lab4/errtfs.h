/**
 * TinyFS error code macros
*/
#ifndef ERROR_TFS_MKFS
#define ERROR_TFS_MKFS -1000 /* Failed to make a file system */
#endif
#ifndef ERROR_TFS_MOUNT
#define ERROR_TFS_MOUNT -1001 /* Failed to make a file system */
#endif
#ifndef ERROR_TFS_UNMOUNT
#define ERROR_TFS_UNMOUNT -1002 /* Failed to unmount a file system */
#endif
#ifndef ERROR_TFS_OPEN
#define ERROR_TFS_OPEN -1003 /* Failed to open a file in the file system */
#endif
#ifndef ERROR_TFS_CLOSE
#define ERROR_TFS_CLOSE -1004 /* Failed to close a file in the file system */
#endif
#ifndef ERROR_TFS_READBYTE
#define ERROR_TFS_READBYTE -1005 /* Failed to read a byte from a file in the file system */
#endif
#ifndef ERROR_TFS_SEEK
#define ERROR_TFS_SEEK -1006 /* Failed to seek a file in the file system */
#endif
#ifndef ERROR_TFS_CREATEFILE
#define ERROR_TFS_CREATEFILE -1007 /* Failed to create a file in the file system */
#endif

/** ERROR CODES BEGIN **/
/* Open disk-related errors*/
#ifndef ERROR_DISK_BLKSIZE
#define ERROR_DISK_BLKSIZE -2000 /* Error due to disk size not being perfectly divisible by the block size */
#endif
#ifndef ERROR_DISK_OPEN
#define ERROR_DISK_OPEN -2001   /* Error opening the disk file */
#endif
#ifndef ERROR_DISK_FD
#define ERROR_DISK_FD -2002     /* Error retrieving the file descriptor for the disk file */
#endif
#ifndef ERROR_DISK_TRUNCATE
#define ERROR_DISK_TRUNCATE -2003    /* Error truncating the disk file */
#endif
#ifndef ERROR_DISK_ACCESS
#define ERROR_DISK_ACCESS -2004    /* Error accessing the disk file, it may not exist in the current directory */
#endif

/* Read/write disk-related errors */
#ifndef ERROR_DISK_FSTAT
#define ERROR_DISK_FSTAT -2005    /* Error accessing the disk file, it may not exist in the current directory */
#endif
#ifndef ERROR_DISK_FOFFSET
#define ERROR_DISK_FOFFSET -2006    /* Error when offset, into the disk, is greater than or equal to the actual file size */
#endif
#ifndef ERROR_DISK_LSEEK
#define ERROR_DISK_LSEEK -2007    /* Error seeking, from an offset, through the disk file */
#endif
#ifndef ERROR_DISK_READ
#define ERROR_DISK_READ -2008    /* Error reading from the disk file */
#endif
#ifndef ERROR_DISK_WRITE
#define ERROR_DISK_WRITE -2009    /* Error writing to the disk file */
#endif
/** ERROR CODES END **/