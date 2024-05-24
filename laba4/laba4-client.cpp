// cd /media/sf_sis_prog/laba4
// ./laba4-client 60002 127.0.0.1
#include "common.hpp"
#include <random>

char message[64];
char buffer[64];
int N = 10;

bool check_exist(std::vector<int> arr, int x)
{
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] == x) return true;
    }
    return false;
}

int main(int argc, char* argv[]) 
{
    srand(time(0));
    auto dest_address = local_addr(atoi(argv[1]), argv[2]);
    std::cout << "addr = " << dest_address << std::endl; 
    int sock_fd = check(make_socket(SOCK_STREAM));
    check(connect(sock_fd, (sockaddr*)&dest_address, sizeof(dest_address)));
    int size =0 ;

    std::vector<int> arr;
    std:: cout << "You need guess value in interval: [1 ; " << N << "]\n";
    std:: cout << "Choose player:\n\tHuman - press 1\n\tMashine - press 0\n";
    int choise = -1;
    while(1)
    {
        std::cin >> choise;
        if (choise == 0 or choise == 1)
            break;
        else 
            std::cout << "Enter right number ";
    }
    while(1)
    {
        size = recv(sock_fd, buffer, sizeof(buffer), MSG_WAITALL);
        check(size);
        if (atoi(buffer) == 2)
        {
            switch (choise)
            {
            case(1):
                std::cout << "Human say the value: ";
                std::cin >> message;
                break;
            case(0):
                int x = rand() % (N - 1 + 1) + 1;
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
                std::cout << "Machine say the value: " << x << std::endl;
                sprintf(message, "%d", x);
                break;
            }
            send(sock_fd, message, sizeof(message), MSG_WAITALL);
        }
        else if(atoi(buffer) == 1)
        {
            std::cout << "I am the BEST GUESSER))";
            break;
        }

    }
    arr.clear();
        
    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);
}