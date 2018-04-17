#ifndef SERVER_H
#define SERVER_H

#include <map>
#include "user.h"
#include "connection.h"

class Server
{
private:
    int tcp_sockfd;
    int udp_sockfd;
    char *data_file_name;
    std::map<int, User> users; // key = card number
    std::map<int, Connection> connections; // key = socketfd, value = card number
    std::map<int, std::pair<int, Balance>> waiting_transactions; 
    fd_set fds;

    void readData();
    void updateData();
    void login(int sockfd, int card_nr, int pin);
    void logout(int sockfd);
    void listSold(int sockfd);
    void transfer(int sockfd, int card_nr, Balance value);
    void unlock();

public:
    Server(int port, char *data_file_name);
    ~Server();
    void run();
};

#endif
