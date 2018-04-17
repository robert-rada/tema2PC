#ifndef USER_H
#define USER_H

#include "balance.h"
#include <cstring>
#include <fstream>

class User
{
public:
    char first_name[13];
    char surname[13];
    int card_nr;
    int pin;
    char password[9];
    Balance balance;
    bool locked;

    User() {};

    User(char first_name[], char surnname[], int card_nr, int pin,
            char password[], Balance balance)
        : card_nr(card_nr), pin(pin), balance(balance)
    {
        strncpy(this->first_name, first_name, 12);
        strncpy(this->surname, surname, 12);
        strncpy(this->password, password, 8);

        first_name[12] = 0;
        surname[12] = 0;
        password[8] = 0;

        locked = false;
    }

    static void parseUser(std::ifstream &in, User &user)
    {
        char buffer[1024];

        in >> buffer;
        strncpy(user.first_name, buffer, 12);
        in >> buffer;
        strncpy(user.surname, buffer, 12);

        in >> user.card_nr >> user.pin;

        in >> buffer;
        strncpy(user.password, buffer, 8);
        in >> user.balance;

        user.first_name[12] = 0;
        user.surname[12] = 0;
        user.password[8] = 0;
    }
};

#endif
