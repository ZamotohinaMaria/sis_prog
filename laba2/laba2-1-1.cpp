#include "./3.Processes/examples/check.hpp"
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <random>
#include <iostream>


using namespace std;

volatile sig_atomic_t last_sig;
volatile sig_atomic_t sig_val;

pid_t child_p, parent_p;
int number;
bool flag;

sigset_t mask;

struct sigaction sig_action;

void rtsig_handler(int signo, siginfo_t* si, void* ctx) {
    last_sig = signo;
    sig_val = si->si_value.sival_int;
}

void guesser()
{
    //int count = 0;
    cout << "im gei" << endl;
    while(true)
    {
        int x = rand() % (10 + 1);
        //sleep(1);
        cout << "Your value = " << x << endl;
        if (sig_val <= 0)
        {
            if (last_sig == SIGUSR2)
            {
                cout << "You not guess." << endl;
                if (flag == true)
                    sigqueue(parent_p, SIGUSR2, sigval{x});
                else
                    sigqueue(child_p, SIGUSR2, sigval{x});
            }
            else if (last_sig == SIGUSR1)
            {
                cout << "YEEEES. YOU DID IT!!!" << endl;
                
                break;
            }
            //pause();
        }
        else
            cout << "111" << sig_val <<endl;
            //pause();
    }
}

void game()
{
    number = rand() % (10 + 1);
    cout << "Number = " << number << endl;
    cout << "child pid g" <<child_p << endl;
    if (child_p != 0) {
        check(sigqueue(child_p, SIGUSR2, sigval{}));
        cout << "lalala" << endl;
    }
    else {
        check(sigqueue(parent_p, SIGUSR2, sigval{}));
    }
    while(true)
    {
        //sleep(1);
        //cout << "abracadabra = " << child_p <<endl;
        cout << "sigggg = " << sig_val << endl;
        //pause();
        if (sig_val > 0)
        {
            if (sig_val == number) {
                if (flag == true) {
                    cout << "fdfdfdfdf" << endl;
                    check(sigqueue(child_p, SIGUSR1, sigval{-1}));
                }
                else {
                    cout << "fdfdfdfdf" << endl;
                    check(sigqueue(parent_p, SIGUSR1, sigval{-1}));

                }
                break;
            }
            else {
                if (flag == true) {
                    check(sigqueue(child_p, SIGUSR2, sigval{-2}));
                    cout << "hehe" << endl;
                }
                else {
                    check(sigqueue(parent_p, SIGUSR2, sigval{-2}));
                    cout << "hehe" << endl;

                }
            }
        }
        else
            cout << "222 = " << sig_val <<endl;
            //pause();
    }
    sleep(1);
    // if (flag == true) flag = false;
    // else flag = true;
}

void child(pid_t parent_p)
{
    int count = 0;
    cout << "hello" << endl;
    sigval x;
    //pause();
    while(true)
    {
        if (last_sig == SIGUSR2)
        {
            cout << "Child not guess. Attempt num: " << count << endl;
            x.sival_int = rand() % (10 + 1);
            sigqueue(parent_p, SIGUSR2, x);
        }
        else if (last_sig == SIGUSR1)
        {
            cout << "YEEEES. YOU DID IT!!!" << endl;
            break;
        }
        if (count < 20) 
            count += 1;
        else
            break;
    }
    
}


int main()
{
    srand(time(0));
    parent_p = getpid();
    sig_action.sa_sigaction = rtsig_handler;
    sig_action.sa_flags = SA_SIGINFO;
    check(sigaction(SIGUSR1, &sig_action, NULL));
    check(sigaction(SIGUSR2, &sig_action, NULL));


    child_p = fork();

    flag = true;
    if (child_p != 0)
        game();
    else
        guesser();
    
    cout << "child pid main" <<child_p << endl;
    // sigval x;
    // x.sival_int = -2;

    // sigqueue(child_p, SIGUSR2, x);
    // sleep(1);
    // child(parent_p);
    // //pause();
    // int count = 0;
    // while(true)
    // {
    //     //pause();
    //     cout << "i'm a bad guy" << endl;
    //     if (sig_val == x.sival_int)
    //     {
    //         cout << "YPEEEEEE" << endl;
    //         sigqueue(child_p, SIGUSR1, x);
    //         break;
    //     }
    //     else 
    //     {
    //         cout << "Please, try again" << endl;
    //         sigqueue(child_p, SIGUSR2, x);
    //     }
    //     if (count < 20) 
    //         count += 1;
    //     else
    //         break;
    // }
    //exit(child_p);
    return 0;
}