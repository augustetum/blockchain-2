#ifndef UTXO_H
#define UTXO_H

#include <string>
using std::string;

class UTXO {
private:
    string transactionId;
    int outputIndex;
    int amount;
    string ownerKey;
    bool spent; //false is unspent, true is spent

public:
    UTXO() : transactionId(""), outputIndex(0), amount(0), ownerKey(""), spent(false) {}

    UTXO(const string& txId, int outIdx, int amt, const string& owner, bool isSpent = false)
        : transactionId(txId), outputIndex(outIdx), amount(amt), ownerKey(owner), spent(isSpent) {}

    string getTransactionId() const { return transactionId; }
    int getOutputIndex() const { return outputIndex; }
    int getAmount() const { return amount; }
    string getOwnerKey() const { return ownerKey; }
    bool isSpent() const { return spent; }

    void setSpent(bool isSpent) { spent = isSpent; }

    string getUTXOId() const {
        return transactionId + ":" + std::to_string(outputIndex);
    }
};

#endif