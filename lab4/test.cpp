#include "tinyFS.h"

using namespace std;

int32_t main(int32_t argc, char* argv[])
{
    printf("Hello world\n");

    char *diskFileName = (char*) "disk1";
    if(argc > 1)
    {
        diskFileName = argv[1];
    }
    // int32_t mkfs_result = tfs_mkfs((char*)"disk1", DEFAULT_DISK_SIZE);
    // cout << "mkfs result is " << mkfs_result << endl;


    int32_t mount_result = tfs_mount(diskFileName);
    cout << "mount result is " << mount_result << endl;

    return 0;
}