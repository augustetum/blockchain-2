#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
using std::string;

class Transaction {
private:
    string senderKey;           
    string receiverKey; 
    double amount;        
    time_t timestamp; 
};

#endif