// Вариант №7
// ROOT
// ├── a_0.bin (227 байт, заполнен 0)
// ├── a_1
// │   └── b_0.bin (символьная ссылка на c_0.bin)
// ├── a_3
// │   └── b_1
// │       └── c_0.bin (пустой файл)
// └── a_2
//     └── b_2
//         ├── c_1 (символьная ссылка на b_1)
//         └── c_2.bin (жесткая ссылка на a_0.bin)
// 

#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

bool Del_Dir()
{
    if (unlink("./ROOT/a_2/b_2/c_2.bin") < 0) 
    {
        cout << "error delete ./ROOT/a_2/b_2/c_2.bin" << endl;
        return 0;
    }
    if (rmdir("./ROOT/a_2/b_2/c_1") < 0) 
    {
        cout << "error delete ./ROOT/a_2/b_2/c_1" << endl;
        return 0;
    }
    if (unlink("./ROOT/a_1/b_0.bin") < 0) 
    {
        cout << "error delete ./ROOT/a_1/b_0.bin" << endl;
        return 0;
    }

    if (unlink("./ROOT/a_0.bin") < 0) 
    {
        cout << "error delete ./ROOT/a_0.bin" << endl;
        return 0;
    }
    if (unlink("./ROOT/a_3/b_1/c_0.bin") < 0) 
    {
        cout << "error delete ./ROOT/a_3/b_1/c_0.bin" << endl;
        return 0;
    }

    if (unlink("./ROOT/a_3/b_1") < 0) 
    {
        cout << "error delete ./ROOT/a_3/b_1" << endl;
        return 0;
    }
    if (unlink("./ROOT/a_2/b_2") < 0) 
    {
        cout << "error delete ./ROOT/a_2/b_2" << endl;
        return 0;
    }


    if (rmdir("./ROOT/a_1") < 0) 
    {
        cout << "error delete ./ROOT/a_1" << endl;
        return 0;
    }

    if (rmdir("./ROOT/a_2") < 0) 
    {
        cout << "error delete ./ROOT/a_2" << endl;
        return 0;
    }
    
    if (rmdir("./ROOT/a_3") < 0) 
    {
        cout << "error delete ./ROOT/a_3" << endl;
        return 0;
    }
    
    if (rmdir("./ROOT") < 0) 
    {
        cout << "error delete ./ROOT" << endl;
        return 0;
    }
    return 1;
}

void Create_Dir(const char* path)
{
    mkdir(path, S_IRWXO|S_IRWXU|S_IRWXG);
}

int main()
{
    // if (Del_Dir() == 0)
    // {
    //     cout << "delet ROOT error" << endl;
    //     exit(2);
    // }
    Create_Dir("./ROOT");
    Create_Dir("./ROOT/a_1");
    Create_Dir("./ROOT/a_2");
    Create_Dir("./ROOT/a_3");
    Create_Dir("./ROOT/a_3/b_1");
    Create_Dir("./ROOT/a_2/b_2");

    unsigned BUFFER_SIZE = 227;
    char buffer[BUFFER_SIZE + 1];
    int fd_a0 = open("./ROOT/a_0.bin", O_CREAT | O_RDWR, S_IRWXO|S_IRWXU|S_IRWXG);
    
    if (fd_a0 < 0)
	{
		fprintf (stderr, "Cannot open file\n");
		exit (1);
	}

    for (int i =0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = '0';
    }

    cout << "size of a_0.bin = " << write(fd_a0, &buffer, BUFFER_SIZE) << endl;

    close(fd_a0);

    int fd_c0 = open("./ROOT/a_3/b_1/c_0.bin", O_CREAT | O_RDWR, S_IRWXO|S_IRWXU|S_IRWXG);

    close(fd_c0);

    int fd_symlink_c0 = open("./ROOT/a_1/b_0.bin", O_CREAT | O_RDWR, S_IRWXO|S_IRWXU|S_IRWXG);
    if (symlink("./ROOT/a_3/b_1/c_0.bin", "./ROOT/a_1/b_0.bin") < 0)
        cout << "c0 link error" << endl;
    
    close(fd_symlink_c0);

    int fd_symlink_b1 = open("./ROOT/a_2/b_2/c_1", O_CREAT | O_RDWR, S_IRWXO|S_IRWXU|S_IRWXG);
    if (symlink("./ROOT/a_3/b_1", "./ROOT/a_2/b_2/c_1") < 0)
        cout << "b1 link error" << endl;

    close(fd_symlink_b1);

    int fd_link_a0 = open("./ROOT/a_2/b_2/c_2.bin", O_CREAT | O_RDWR, S_IRWXO|S_IRWXU|S_IRWXG);
    if (link("./ROOT/a_0.bin", "./ROOT/a_2/b_2/c_1.bin") < 0)
        cout << "a0 link error" << endl;

    close(fd_link_a0);
    
    return 0;
}
