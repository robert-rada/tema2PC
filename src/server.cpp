#include "server.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>

const int MAX_CLIENTS = 100;

Server::Server(int port, char *data_file_name)
    : data_file_name(data_file_name)
{
    std::cerr << "Starting server...\n";

    FD_ZERO(&fds);

    // build address
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // TCP
    tcp_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (tcp_sockfd < 0)
        std::cerr << "Error:  socket()  for TCP\n";

    if (bind(tcp_sockfd, (sockaddr*) &addr, sizeof(sockaddr_in)) < 0)
        std::cerr << "Error:  bind()  for TCP\n";

    listen(tcp_sockfd, MAX_CLIENTS);

    // UDP
    udp_sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd < 0)
        std::cerr << "Error:  socket()  for UDP\n";

    if (bind(udp_sockfd, (sockaddr*) &addr, sizeof(sockaddr_in)) < 0)
        std::cerr << "Error:  bind()  for UDP\n";

    FD_SET(tcp_sockfd, &fds);
    FD_SET(udp_sockfd, &fds);
    FD_SET(0, &fds);

    readData();
}

void Server::run()
{
    static const int BUFLEN = 1024;
    char buffer[BUFLEN];
    int fdmax = std::max(tcp_sockfd, udp_sockfd);
    fd_set tmp_fds;

    bool running = true;
    while (running)
    {
        tmp_fds = fds;

        if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1)
            std::cerr << "Error:  select() \n";

        for (int i = 0; i <= fdmax; i++)
            if (FD_ISSET(i, &tmp_fds))
            {
                if (i == 0)
                {
                    // Read from stdin
                    std::cin >> buffer;
                    if (strcmp(buffer, "quit") == 0)
                        running = false;

                    // notice clients???
                }
                else if (i == tcp_sockfd)
                {
                    // Accept connection
                    sockaddr client_addr;
                    socklen_t client_len;
                    int newsockfd = accept(tcp_sockfd, (sockaddr*) &client_addr,
                            &client_len);

                    if (newsockfd == -1)
                        std::cerr << "Error:  accept() \n";
                    else
                    {
                        std::cerr << "Accepted connection (socket = " <<
                            newsockfd << ")\n";

                        FD_SET(newsockfd, &fds);
                        fdmax = std::max(fdmax, newsockfd);

                        connections.insert( std::make_pair(newsockfd,
                                    Connection(-1)) );
                    }
                }
                else if (i == udp_sockfd)
                {
                    // UDP message
                }
                else
                {
                    // TCP message 
                    memset(buffer, 0, sizeof(buffer));
                    int recv_len = recv(i, buffer, sizeof(buffer), 0);

                    if (recv_len < 0)
                    {
                        std::cerr << "Error: recv()\n";
                        continue;
                    }
                    else if (recv_len == 0)
                    {
                        std::cout << "Socket " << i << " closed\n";
                        close(i);
                        FD_CLR(i, &fds);

                        continue;
                    }

                    if (strstr(buffer, "login") != NULL)
                        login(i, *(int*)(buffer + 5), *(int*)(buffer + 9));
                    else if (strstr(buffer, "logout") != NULL)
                        logout(i);
                    else if (strstr(buffer, "listsold") != NULL)
                        listSold(i);
                    else if (strstr(buffer, "transfer") != NULL)
                        transfer(i, *(int*)(buffer + 8), *(Balance*)(buffer + 12));
                    else if (strstr(buffer, "quit") != NULL)
                        running = false;
                    else
                        std::cerr << "Error: Unknown request: " << buffer <<
                            '\n';
                }
            }
    }
}

void Server::login(int sockfd, int card_nr, int pin)
{
    char buffer[64];
    memset(buffer, 0, sizeof(buffer));

    if (connections[sockfd].card_nr >= 0)
        *(int*)buffer = -2; // client is already logged in
    else if (users.find(card_nr) == users.end())
        *(int*)buffer = -4; // card not found
    else if (users[card_nr].locked)
        *(int*)buffer = -5; // card is locked
    else if (pin != users[card_nr].pin)
    {
        *(int*)buffer = -3; // wrong pin
        int tries = ++connections[sockfd].login_tries;
        if (tries >= 3)
            users[card_nr].locked = true;
    }
    else
    {
        // ok
        strcpy(buffer+4, "Welcome ");
        strcat(buffer+4, users[card_nr].first_name);
        strcat(buffer+4, " ");
        strcat(buffer+4, users[card_nr].surname);
        strcat(buffer+4, "\n");

        connections[sockfd].card_nr = card_nr;
    }

    send(sockfd, buffer, sizeof(buffer), 0);
}

void Server::logout(int sockfd)
{
    char buffer[64];
    memset(buffer, 0, sizeof(buffer));

    if (connections[sockfd].card_nr < 0)
        *(int*)buffer = -1;
    else
    {
        connections[sockfd] = Connection();
        connections.erase(connections.find(sockfd));
        connections.insert( std::make_pair(sockfd, Connection()) );
        strcpy(buffer+4, "Clientul a fost deconectat\n");
    }

    send(sockfd, buffer, sizeof(buffer), 0);
}

void Server::listSold(int sockfd)
{
}

void Server::transfer(int sockfd, int card_nr, Balance value)
{
}

void Server::readData()
{
    std::ifstream in(data_file_name);

    int n;
    in >> n;

    for (int i = 0; i < n; i++)
    {
        User user;
        User::parseUser(in, user);
        users.insert( std::make_pair(user.card_nr, user) );
    }

    in.close();
}

void Server::updateData()
{
    std::ofstream out(data_file_name);

    out << users.size() << '\n';

    for (const auto &it: users)
    {
        const User &user = it.second;
        out << user.first_name << ' ';
        out << user.surname << ' ';
        out << user.card_nr << ' ';
        out << user.pin << ' ';
        out << user.password << ' ';
        out << user.balance << '\n';
    }

    out.close();
}

Server::~Server()
{
    std::cerr << "Closing server...\n";

    close(tcp_sockfd);
    close(udp_sockfd);

    updateData();
}
