#ifndef CONNECTION_H
#define CONNECTION_H

class Connection
{
public:
    int card_nr;
    int login_tries;
    bool waiting_transaction;
    int transaction_card;
    Balance transaction_amount;

    Connection(int card_nr)
        : card_nr(card_nr), login_tries(0), waiting_transaction(false) {};
    Connection()
        : card_nr(-1), login_tries(0), waiting_transaction(false) {};
};

#endif
