#include "./3.Processes/examples/check.hpp"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <wait.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <filesystem>
#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#define N 10

using namespace std;

pid_t child_p, parent_p;
bool flag;

int sig1 = -1;
int sig2 = -2;

bool check_exist(vector<int> arr, int x)
{
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] == x) return true;
    }
    return false;
}

void my_sleep()
{
    sleep(1);
}

void guesser(int fd_write, int fd_read)
{
    int count = 0;
    int buffer, x;
    vector<int> arr;
    if (child_p != 0)
        cout << "i'm geniy, i'm a parent" << endl;
    else
        cout << "i'm geniy, i'm a child" << endl;
    while(true)
    {
        read(fd_read, &buffer, sizeof(int));
        //cout << "guesser buffer = " << buffer << endl;
        if (buffer == sig2)
        {
            x = rand() % (N - 1 + 1) + 1;
            while(true)
            {
                if (check_exist(arr, x) == true)
                    x = rand() % (N - 1 + 1) + 1;
                else 
                    {
                        arr.push_back(x);
                        break;
                    }
            }
            cout << "My value = " << x << endl;
            count += 1;
            write(fd_write, &x, sizeof(x));
        }
        else if (buffer == sig1)
        {
            cout << "I need " << count << " attempts to win" << endl;
            break;
        }
           
    }
    if (flag == true) flag = false;
    else flag = true;
    arr.clear();
}

void game(int fd_write, int fd_read)
{
    if (child_p != 0)
        cout << "I'm a parent ";
    else
        cout << "I'm a child ";
    int number = rand() % (N - 1 + 1) + 1;
    cout << "I'm wish a number = " << number << endl;

    write(fd_write, &sig2, sizeof(sig2));
    int buffer;
    while(true)
    {
        read(fd_read, &buffer, sizeof(int));
        //cout << "game buffer = " << buffer << endl;
        if (buffer == number) {
            write(fd_write, &sig1, sizeof(sig1));
            cout << "YEEEES. YOU DID IT!!!" << endl;
            break;
        }
        else {
            cout << "You not guess." << endl;
            write(fd_write, &sig2, sizeof(sig2));
        }
        my_sleep();
    }
    if (flag == true) flag = false;
    else flag = true;
}

const char PIPE_P_TO_C[] = "/tmp/p_to_c";
const char PIPE_C_TO_P[] = "/tmp/c_to_p";


int main()
{
    parent_p = getpid();
    //check(mkfifo("/tmp/myfifo", S_IRUSR|S_IWUSR));
 
    unlink(PIPE_C_TO_P);
    unlink(PIPE_P_TO_C);
    check(mkfifo(PIPE_C_TO_P, S_IRWXU|S_IRWXG|S_IRWXO) );
    check(mkfifo(PIPE_P_TO_C, S_IRWXU|S_IRWXG|S_IRWXO) );

    child_p = check(fork());

    int fd_write, fd_read;

    if (child_p != 0)
        cout << "parent pid = " << getpid() << "\nchild pid = " << child_p << endl;

    if (child_p != 0)
    {
        fd_write = check(open(PIPE_P_TO_C, O_WRONLY));
        fd_read = check(open(PIPE_C_TO_P, O_RDONLY));
    }
    else
    {
        fd_read = check(open(PIPE_P_TO_C, O_RDONLY));
        fd_write = check(open(PIPE_C_TO_P, O_WRONLY));
    } 

    
    flag = true;
    for (int i = 1; i <= 10; i++)
    {
        if (child_p != 0)
        {
            cout << "\n\nGame № " << i << endl;
            srand(time(0));
            if (flag == true)
                game(fd_write, fd_read);
            else
            {
                guesser(fd_write, fd_read);
                //cout << "You need " << count << " attempts to win";
            }

        }
        else
        {
            srand(time(0) + 100);
            if (flag == true)
            {
                guesser(fd_write, fd_read);
            }
            else
                game(fd_write, fd_read);
        } 
        int buffer;
        //read(fd_read, &buffer, sizeof(buffer));
        my_sleep();
    }
    return 0;
}