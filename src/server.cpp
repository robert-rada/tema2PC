#include "server.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
        std::cerr << "Error on socket() call for TCP\n";

    if (bind(tcp_sockfd, (sockaddr*) &addr, sizeof(sockaddr_in)) < 0)
        std::cerr << "Error on bind() call for TCP\n";

    listen(tcp_sockfd, MAX_CLIENTS);

    // UDP
    udp_sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd < 0)
        std::cerr << "Error on socket() call for UDP\n";

    if (bind(udp_sockfd, (sockaddr*) &addr, sizeof(sockaddr_in)) < 0)
        std::cerr << "Error on bind() call for UDP\n";

    readData();
}

void Server::run()
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
