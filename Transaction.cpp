#include "Transaction.h"
#include <iostream>
#include "customGenerator.h"

using namespace std;


Transaction::Transaction(const string& from, const string& to, const std::vector<UTXO>& ins, const std::vector<UTXO>& outs)
    : senderKey(from), receiverKey(to), inputs(ins), outputs(outs) {
    transactionID = calculateTransactionId();

    for (size_t i = 0; i < outputs.size(); ++i) {
        outputs[i] = UTXO(transactionID, i, outputs[i].getAmount(), outputs[i].getOwnerKey(), false);
    }
}

string Transaction::getSender() const { return senderKey; }
string Transaction::getReceiver() const { return receiverKey; }

double Transaction::getAmount() const {
    double total = 0;
    for (const auto& output : outputs) {
        total += output.getAmount();
    }
    return total;
}

string Transaction::getTransactionId() const { return transactionID; }

string Transaction::calculateTransactionId() const {
    HashGenerator hasher;
    string data = senderKey + receiverKey;

    for (const auto& input : inputs) {
        data += input.getTransactionId() + to_string(input.getOutputIndex()) + to_string(input.getAmount());
    }

    for (const auto& output : outputs) {
        data += output.getOwnerKey() + to_string(output.getAmount());
    }

    string hash = hasher.generateHash(data);
    return hash;
}

const std::vector<UTXO>& Transaction::getInputs() const {
    return inputs;
}

const std::vector<UTXO>& Transaction::getOutputs() const {
    return outputs;
}

void Transaction::setInputs(const std::vector<UTXO>& inputUTXOs) {
    inputs = inputUTXOs;
}

void Transaction::setOutputs(const std::vector<UTXO>& outputUTXOs) {
    outputs = outputUTXOs;
}

void Transaction::addInput(const UTXO& input) {
    inputs.push_back(input);
}

void Transaction::addOutput(const UTXO& output) {
    outputs.push_back(output);
}


void Transaction::print() const {
    cout << "TX: " << transactionID << endl;
    cout << "  From: " << senderKey << endl;
    cout << "  To: " << receiverKey << endl;

    cout << "  Inputs (" << inputs.size() << "):" << endl;
    int totalInput = 0;
    for (const auto& input : inputs) {
        cout << "    - UTXO " << input.getUTXOId()
             << " : " << input.getAmount() << endl;
        totalInput += input.getAmount();
    }
    cout << "  Total input: " << totalInput << endl;

    cout << "  Outputs (" << outputs.size() << "):" << endl;
    int totalOutput = 0;
    int transactionAmount = 0;
    for (size_t i = 0; i < outputs.size(); ++i) {
        const auto& output = outputs[i];
        bool isChange = (output.getOwnerKey() == senderKey);

        cout << "    - To " << output.getOwnerKey().substr(0, 16) << "... : "
             << output.getAmount();

        if (isChange) {
            cout << " (change)";
        } else {
            transactionAmount += output.getAmount();
        }
        cout << endl;
        totalOutput += output.getAmount();
    }
    cout << "  Total output: " << totalOutput << endl;
    cout << "  Transaction amount: " << transactionAmount << endl;
}