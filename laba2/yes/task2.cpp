#include "check.hpp"
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <ctime>

using namespace std;

void sigaction_handle(int signum, siginfo_t *info, void* ctx);

int pid1, pid2, value = 0;
bool isParent = true;
string player_name;

volatile sig_atomic_t last_sig;
volatile sig_atomic_t sig_value;

struct sigaction act;

void sigaction_handle(int signum, siginfo_t *info, void* ctx) {
    sig_value = info->si_value.sival_int;
    last_sig = signum;
}

void pick_a_number() {
    value = rand() % 10;
    cout << player_name << ": My value is " << value << "\n";
}

int get_value() {
    return rand() % 10;
}

void guesser() {
    while (true) {
        int tmp = get_value();
        cout << player_name << ": Is it " << tmp << "?\n";
        sleep(1);
        if (pid2 == 0) {
            check(sigqueue(pid1, SIGUSR2, sigval{tmp}));
        }
        else {
            check(sigqueue(pid2, SIGUSR2, sigval{tmp}));
        }
        sleep(1);
        cout << "last sig = " <<last_sig << endl;
        if (last_sig == SIGUSR1) {
            if (pid2 == 0) cout << "Child process guessed value!\n\n";
            else cout << "Parent process guessed value!\n\n";
            if (pid2 == 0) {
                check(sigqueue(pid1, SIGUSR1, sigval{tmp}));
            }
            else {
                check(sigqueue(pid2, SIGUSR1, sigval{tmp}));
            }
            break;
        }
    }
}

void game(bool flag) {
    if ((isParent && flag) || (!isParent && !flag)) {
        if (flag) {
            cout << "Parent process picks a number, child process guesses.\n";
            player_name = "Player 1";
        }
        else {
            cout << "Child process picks a number, parent process guesses.\n";
            player_name = "Player 2";
        }
        pick_a_number();
        if (flag) {
            check(sigqueue(pid2, SIGUSR2, sigval{}));
        }
        else {
            check(sigqueue(pid1, SIGUSR2, sigval{}));
        }
        while (true) {
            sleep(1);
            //pause();
            cout << "abracadabra" << endl;
            if (sig_value == value) {
                if (flag) {
                    cout << "fdfdfdfdf" << endl;
                    check(sigqueue(pid2, SIGUSR1, sigval{}));
                }
                else {
                    cout << "fdfdfdfdf" << endl;
                    check(sigqueue(pid1, SIGUSR1, sigval{}));

                }
                break;
            }
            else {
                if (flag) {
                    check(sigqueue(pid2, SIGUSR2, sigval{}));
                    cout << "hehe" << endl;
                }
                else {
                    check(sigqueue(pid1, SIGUSR2, sigval{}));
                    cout << "hehe" << endl;

                }
            }
        }
        sleep(1);
        if (flag) isParent = false;
        else isParent = true;
    }
    else
    {
        if(flag){
            player_name = "Player 1";
        }
        else {
            player_name = "Player 2";
        }
        sleep(1);
        cout << "lalala" << endl;
        guesser();
        if (flag) isParent = true;
        else isParent = false;
    }
}

int main() {
    srand(11 + time(NULL));
    pid1 = getpid();
    act.sa_sigaction = sigaction_handle;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &act, 0);  // didn't guess
    sigaction(SIGUSR1, &act, 0); // guessed
    sigaction(SIGRTMIN, &act, 0); // values
    pid2 = fork();

    for (int i = 0; i < 5; i++) {
        cout << "pid2 = " << pid2;
        if (pid2 != 0) {                // parent process
            game(true);
        }
        else {                          // child process
            srand(time(NULL));
            game(false);
        }
    }
}