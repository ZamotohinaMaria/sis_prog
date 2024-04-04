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

void Create_Dir(const char* path)
{
    if (fs::exists(path))
        cout << path <<" - exists" << endl;
    else
        mkdir(path, S_IRWXO|S_IRWXU|S_IRWXG);
}

int main()
{
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

    cout << write(fd_a0, &buffer, BUFFER_SIZE) << endl;

    int fd_c0 = open("./ROOT/a_3/b_1/c_0.bin", O_CREAT | O_RDWR, S_IRWXO|S_IRWXU|S_IRWXG);

    int fd_symlink_c0 = open("./ROOT/a_1/b_0.bin", O_CREAT | O_RDWR, S_IRWXO|S_IRWXU|S_IRWXG);
    if (symlink("./ROOT/a_3/b_1/c_0.bin", "./ROOT/a_1/b_0.bin") < 0)
        cout << "c0 link error" << endl;

    int fd_symlink_b1 = open("./ROOT/a_2/b_2/c_1.bin", O_CREAT | O_RDWR, S_IRWXO|S_IRWXU|S_IRWXG);
    if (symlink("./ROOT/a_3/b_1", "./ROOT/a_2/b_2/c_1.bin") < 0)
        cout << "b1 link error" << endl;

    int fd_link_a0 = open("./ROOT/a_2/b_2/c_2.bin", O_CREAT | O_RDWR, S_IRWXO|S_IRWXU|S_IRWXG);
    if (link("./ROOT/a_0.bin", "./ROOT/a_2/b_2/c_1.bin") < 0)
        cout << "a0 link error" << endl;
    return 0;
}
