#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <netinet/in.h>
#include "user.h"
#include "connection.h"

class Address
{
public:
    sockaddr_in addr;

    Address(sockaddr_in addr)
        : addr(addr) {};

    bool operator< (const Address &other) const
    {
        return this->addr.sin_port < other.addr.sin_port;
    }
};

class Server
{
private:
    int tcp_sockfd;
    int udp_sockfd;
    char *data_file_name;
    fd_set fds;

    std::map<int, User> users; // key = card number
    std::map<int, Connection> connections; // key = socketfd, value = card number
    std::map<Address, int> waiting_unlocks;

    void readData();
    void printData();
    void updateData();
    void login(int sockfd, int card_nr, int pin);
    void logout(int sockfd);
    void listSold(int sockfd);
    void transfer(int sockfd, int card_nr, Balance value);
    void unlock(int card_nr, sockaddr_in &client_addr);
    void unlockResponse(sockaddr_in &client_addr, char *password);

public:

    Server(int port, char *data_file_name);
    ~Server();
    void run();
};

#endif
