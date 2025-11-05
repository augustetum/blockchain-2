#include <iostream>
using std::string;

class UTXO {
private:
    string transactionId;
    int outputIndex;
    int amount;
    string ownerKey;
    bool spent; //false is unspent, true is spent
};