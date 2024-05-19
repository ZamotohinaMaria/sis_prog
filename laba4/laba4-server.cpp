// ./laba4-server 60002 127.0.0.1
#include "common.hpp"
#include <random>

char buffer[64];
char message[64];

int N =10;
int flag = 0;
int number;
int main(int argc, char* argv[]) 
{
    srand(time(0));
    sockaddr_in  server_address = local_addr(atoi(argv[1]), argv[2]);
    int listening_socket = check(make_socket(SOCK_STREAM));
    int connected_socket = 0;
    check(bind(listening_socket, (sockaddr*)&server_address, sizeof(server_address)));
    check(listen(listening_socket, 1));

    while (true) {
        std::cout << "\nServer wait" << std::endl;
        sockaddr_in connected_address{};
        socklen_t addrlen = sizeof(connected_address);
        connected_socket = check(accept(listening_socket, (sockaddr*)&connected_address, &addrlen));
        std::cout << "Connected from " << connected_address << std::endl;
        
        while (true) 
        {
            if (flag == 0)
            {
                number = rand() % (N - 1 + 1) + 1;
                std::cout << "I'm wish a number = " << number << std::endl;
                sprintf(message, "%d", 2);
                send(connected_socket, message, sizeof(message), MSG_WAITALL);
                flag = 1;
            }
            int size = recv(connected_socket, buffer, sizeof(buffer), MSG_WAITALL);
            if (size == 0 || (size < 0 && errno == ENOTCONN))
                break; //disconnected
            check(size);
            std::cout
                << connected_address
                << " sent a message of a size " << size
                << ":" << std::string_view(buffer, size) << std::endl;
            if (atoi(buffer) == number)
            {
                std::cout << "You GUESS!\n\n";
                sprintf(message, "%d", 1);
                send(connected_socket, message, sizeof(message), MSG_WAITALL);
            }
            else
            {
                std::cout << "You not guess(((\n\n";
                sprintf(message, "%d", 2);
                send(connected_socket, message, sizeof(message), MSG_WAITALL);
                
            }
        }
        if (flag == 1) flag = 0;
        std::cout << "Disconnected from " << connected_address << std::endl;
    }
    close(connected_socket);
}