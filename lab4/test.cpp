#include "tinyFS.h"

using namespace std;

int main()
{
    printf("Hello world\n");

    int result = tfs_mkfs((char*)"disk1", DEFAULT_DISK_SIZE);
    cout << "result is " << result << endl;
    //tfs_mount("disk1")
}