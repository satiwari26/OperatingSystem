#include "tinyFS.h"

using namespace std;

int main(int argc, char* argv[])
{
    printf("Hello world\n");

    char *diskFileName = (char*) "disk1";
    if(argc > 1)
    {
        diskFileName = argv[1];
    }
    cout << "size datablock:" << sizeof(dataBlock) << endl;
    int mkfs_result = tfs_mkfs((char*)diskFileName, DEFAULT_DISK_SIZE);
    cout << "mkfs result is " << mkfs_result << endl;


    int mount_result = tfs_mount((char*) diskFileName);
    cout << "mount result is " << mount_result << endl;

    int open_result = tfs_open((char*) "file2");
    cout << "open result is " << open_result << endl;

    int write_result = tfs_write(open_result, (char*)"cooked", 7);
    cout << "write result is " << write_result << endl;

    for (int i = 0; i < 7; i++)
    {
        char buffer= '\0';
        int readbyte_result = tfs_readByte(open_result, &buffer);
        cout << "read result is " << readbyte_result << " with buffer " << buffer << endl;
    }
    
    int unmount_result = tfs_unmount();
    cout << "unmount result is " << unmount_result << endl;

    return 0;
}