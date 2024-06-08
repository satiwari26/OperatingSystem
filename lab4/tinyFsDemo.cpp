#include "libTinyFS.h"

using namespace std;

int main(int argc, char* argv[])
{
    printf("Hello world\n");

    char *diskFileName = (char*) "disk1";
    if(argc > 1)
    {
        diskFileName = argv[1];
    }

    int mkfs_result = tfs_mkfs((char*)diskFileName, DEFAULT_DISK_SIZE);
    cout << "mkfs result is " << mkfs_result << endl;

    int mount_result = tfs_mount((char*) diskFileName);
    cout << "mount result is " << mount_result << endl;

    int open_result = tfs_open((char*) "file2");
    cout << "open result is " << open_result << endl;

    int open_result3 = tfs_open((char*) "file3");
    cout << "open result3 is " << open_result3 << endl;

    int open_result4 = tfs_open((char*) "file4");
    cout << "open result4 is " << open_result4 << endl;

    tfs_readdir();

    int rename_res = tfs_rename(open_result, (char*)"file5");

    tfs_readdir();

    int write_result = tfs_write(open_result, (char*)"cooked", 7);
    cout << "write result is " << write_result << endl;

    for (int i = 0; i < 7; i++)
    {
        char buffer= '\0';
        int readbyte_result = tfs_readByte(open_result, &buffer);

        if (readbyte_result < 0)
        {
            cout << "error: result is : " << readbyte_result << endl;
        }
        cout << "buffer " << buffer << endl;
    }  


    char buffer2 = '0';
    int seek_result = tfs_seek(open_result, 3);
    cout << "seek result is " << seek_result << " with buffer " << buffer2 << endl;

    int readbyte_result = tfs_readByte(open_result, &buffer2);
    cout << "read result is " << readbyte_result << " with buffer " << buffer2 << endl;

    char test[300] = {'a'};
    for (int i = 0; i < 300; i++) 
    {
        test[i] = 'a';
    }
    int write_result2 = tfs_write(open_result, (char*)&test[0], 300);
    cout << "write result is " << write_result2 << endl;
    for (int i = 0; i < 300; i++)
    {
        char buffer = '\0';
        int readbyte_result = tfs_readByte(open_result, &buffer);
        if (readbyte_result < 0)
        {
            cout << "error: result is : " << readbyte_result << endl;
        }
        cout << "buffer " << buffer << endl;
    }
    
    inode stat = tfs_stat(open_result);
    cout << stat.creationTime << endl;
    
    int delete_result = tfs_delete(open_result);
    cout << "delete result is " << delete_result << endl;

    int unmount_result = tfs_unmount();
    cout << "unmount result is " << unmount_result << endl;

    return 0;
}