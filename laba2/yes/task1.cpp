#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string>
#include "check.hpp"
#include <iostream>
using namespace std;

int pid1, pid2, value = 0;
bool isParentWish = true;
string player_name;

void pick_a_number() {
    value = rand() % 10;
    cout << player_name << ": My value is " << value << "\n";
}

int get_value() {
    return rand() % 10;
}

void guesser(mqd_t mqd) {
    while (true) {
        int tmp = get_value();
        cout << player_name << ": Is it " << tmp << "?\n";
        sleep(1);
        check(mq_send(mqd, (char*)&tmp, sizeof(tmp), 0));
        int* buf = new int[8 * 1024 / sizeof(int)];
        check(mq_receive(mqd, (char*)buf, 8*1024, NULL));
        if (buf[0] == -1) {
            if (pid2 == 0) cout << "Child process guessed value!\n\n";
            else cout << "Parent process guessed value!\n\n";
            break;
        }
    }
}

void game(bool flag, mqd_t mqd) {
    if ((isParentWish && flag) || (!isParentWish && !flag)) {
        if (flag) {
            cout << "Parent process picks a number, child process guesses.\n";
            player_name = "Player 1";
        }
        else {
            cout << "Child process picks a number, parent process guesses.\n";
            player_name = "Player 2";
        }
        pick_a_number();
        sleep(1);
        while (true) {
            int* buf = new int[8*1024/sizeof(int)];
            check(mq_receive(mqd, (char*)buf, 8*1024, 0));
            if (buf[0] == value) {
                int b = -1;
                check(mq_send(mqd, (char*)&b, sizeof(b), 0));
                sleep(1);
                break;
            }
            else {
                int b = -2;
                check(mq_send(mqd, (char*)&b, sizeof(b), 0));
                sleep(1);
            }
        }
        if (flag) isParentWish = false;
        else isParentWish = true;
    }
    else
    {
        if(flag) {
            player_name = "Player 1";
        }
        else {
            player_name = "Player 2";
        }
        guesser(mqd);
        if (flag) isParentWish = true;
        else isParentWish = false; // isParentWish = flag
    }
}

int main() {
    srand(11 + time(NULL));
    pid1 = getpid();
    auto d = check(mq_open("/mq_file", O_CREAT, S_IRUSR | S_IWUSR, NULL));
    check(mq_close(d));
    pid2 = fork();

    for (int i = 0; i < 5; i++) {
        if (pid2 != 0) {                            // parent process
            mqd_t mqd = check(mq_open("/mq_file", O_RDWR));
            if (mqd == -1) return -1;
            game(true, mqd);

        }
        else {                                    // child process
            srand(10 + time(NULL));
            mqd_t mqd = check(mq_open("/mq_file", O_RDWR));
            if (mqd == -1) return -1;
            game(false, mqd);
        }
    }
}
