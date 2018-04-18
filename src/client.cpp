#include "server.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>

std::string errors[] = {"", "-1: Clientul nu este autentificat\n",
                            "-2: Sesiune deja deschisa\n",
                            "-3: Pin gresit\n",
                            "-4: Numar card inexistent\n",
                            "-5: Card blocat\n",
                            "-6: Operatie esuata\n",
                            "-7: Deblocare esuata\n",
                            "-8: Fonduri insuficiente\n",
                            "-9: Operatie anulata\n",
                            "-10: Eroare la apel "};

std::ofstream log_file("log.txt");

void log(std::string message)
{
    std::cout << message;
    log_file << message;
}

void log(const char *message)
{
    std::cout << message;
    log_file << message;
}

int initTCPSocket(char server_addr[], int server_port)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    inet_aton(server_addr, &addr.sin_addr);
    
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        log(errors[10] + "socket()\n");

    if (connect(sockfd, (sockaddr*) &addr, sizeof(addr)) < 0)
        log(errors[10] + "connect()\n");

    return sockfd;
}

int receiveResponse(int tcp_sockfd)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int count = recv(tcp_sockfd, buffer, sizeof(buffer), 0);
    if (count > 0)
    {
        if (*(int*)buffer != 0)
        {
            log(errors[-1 * ((int*)buffer)[0]]);
            log("\n");
        }
        else
        {
            log("IBANK> ");
            log(buffer + 4);
            log("\n");
        }
    }
    else if (count == 0)
    {
        log ("Connection closed\n");
        return -1;
    }
    else
    {
        log (errors[10] + " recv()\n");
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " IP_server port_server\n";
        exit(1);
    }

    int tcp_sockfd = initTCPSocket(argv[1], atoi(argv[2]));

    char buffer[1024];
    bool running = true;
    while (running)
    {
        std::string command;
        std::cin >> command;
        log_file << command;

        strcpy(buffer, command.c_str());
        int comm_len = command.size();
        char *data = (buffer + strlen(buffer));
        if (command == "login")
        {
            int card_nr, pin;
            std::cin >> card_nr >> pin;
            log_file << ' ' << card_nr << ' ' << pin << '\n';

            ((int*)data)[0] = card_nr;
            ((int*)data)[1] = pin;
        }
        else if (command == "logout")
        {
        }
        else if (command == "listsold")
        {
        }
        else
            log("Unknown command: " + command + '\n');

        int count = send(tcp_sockfd, buffer, comm_len + 8, 0);
        if (count < 0)
            log(errors[10] + " send()\n");

        if (receiveResponse(tcp_sockfd) < 0)
            running = false;
    }

    log_file.close();

    return 0;
}

