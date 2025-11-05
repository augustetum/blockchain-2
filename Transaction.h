#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>
#include "UTXO.h"
using std::string;

class Transaction {
private:
    string senderKey;           
    string receiverKey; 
    std::vector<UTXO> inputs;
    std::vector<UTXO> outputs;

public:
    string transactionID;

    Transaction(const string& from, const string& to, const std::vector<UTXO>& ins, const std::vector<UTXO>& outs);
    
    string getSender() const;
    string getReceiver() const;
    double getAmount() const;
    string getTransactionId() const;

    const std::vector<UTXO>& getInputs() const;
    const std::vector<UTXO>& getOutputs() const;

    void setInputs(const std::vector<UTXO>& inputUTXOs);
    void setOutputs(const std::vector<UTXO>& outputUTXOs);
    void addInput(const UTXO& input);
    void addOutput(const UTXO& output);

    string calculateTransactionId() const;

    void print() const;
};

#endif