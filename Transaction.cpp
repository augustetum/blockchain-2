#include "Transaction.h"
#include <iostream>

using namespace std;

Transaction::Transaction(const string& from, const string& to, double amt)
    : senderKey(from), receiverKey(to), amount(amt) {
    timestamp = time(nullptr);
    transactionID = calculateTransactionId();
}

string Transaction::getSender() const { return senderKey; }
string Transaction::getReceiver() const { return receiverKey; }
double Transaction::getAmount() const { return amount; }
time_t Transaction::getTimestamp() const { return timestamp; }
string Transaction::getTransactionId() const { return transactionID; }

// string Transaction::calculateTransactionId() const {
//     string data = senderKey + receiverKey + 
//                   to_string(amount) + 
//                   to_string(timestamp);
//     return hash(data);  //cia reikia musu hasho
// }


void Transaction::print() const {
    cout << "TX: " << transactionID << endl;
    cout << "  From: " << senderKey << endl;
    cout << "  To:   " << receiverKey << endl;
    cout << "  Amount: " << amount << endl;
}