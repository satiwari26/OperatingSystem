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

    int unmount_result = tfs_unmount();
    cout << "unmount result is " << unmount_result << endl;

    return 0;
}