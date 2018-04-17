#ifndef CONNECTION_H
#define CONNECTION_H

class Connection
{
public:
    int card_nr;
    int login_tries;
    bool waiting_transaction;

    Connection(int card_nr)
        : card_nr(card_nr), login_tries(0), waiting_transaction(false) {};
    Connection() { Connection(-1); };
};

#endif
