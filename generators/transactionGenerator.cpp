#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include "../User.h"
#include "../customGenerator.h"
#include "../Transaction.h"
#include "../UTXO.h"
using std::string;

const int TRANSACTION_AMOUNT = 10000;

std::vector<std::string> loadUsersFromFile(const std::string& filename) {
    std::vector<std::string> userPublicKeys;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return userPublicKeys;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name, publicKey;
        int balance;
        iss >> name >> publicKey >> balance;
        
        userPublicKeys.push_back(publicKey);
    }
    file.close();
    
    std::cout << "Loaded " << userPublicKeys.size() << " users from file" << std::endl;
    return userPublicKeys;
}

std::unordered_map<std::string, std::vector<UTXO>> loadUTXOsFromFile(const std::string& filename) {
    std::unordered_map<std::string, std::vector<UTXO>> utxosByOwner;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return utxosByOwner;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string txId, ownerKey, spentStr;
        int outputIndex, amount;
        
        iss >> txId >> outputIndex >> amount >> ownerKey >> spentStr;
        
        bool spent = (spentStr == "true");
        
        if (!spent) {
            UTXO utxo(txId, outputIndex, amount, ownerKey, spent);
            utxosByOwner[ownerKey].push_back(utxo);
        }
    }
    file.close();
    
    std::cout << "Loaded UTXOs for " << utxosByOwner.size() << " users" << std::endl;
    return utxosByOwner;
}

std::vector<Transaction> generateTransactions(
    const std::vector<std::string>& userPublicKeys,
    std::unordered_map<std::string, std::vector<UTXO>>& utxosByOwner,
    int count) {
    
    std::vector<Transaction> transactions;
    std::mt19937 gen(time(NULL));
    std::uniform_int_distribution<> userDist(0, userPublicKeys.size() - 1);

    int i = 0;
    while (i < count) {
        int senderIdx = userDist(gen);
        std::string senderKey = userPublicKeys[senderIdx];

        if (utxosByOwner[senderKey].empty()) {
            continue;
        }

        i++;
        
        int receiverIdx;
        do {
            receiverIdx = userDist(gen);
        } while (receiverIdx == senderIdx);
        std::string receiverKey = userPublicKeys[receiverIdx];
        
        std::uniform_int_distribution<> utxoDist(0, utxosByOwner[senderKey].size() - 1);
        int utxoIdx = utxoDist(gen);
        UTXO inputUTXO = utxosByOwner[senderKey][utxoIdx];
        
        utxosByOwner[senderKey].erase(utxosByOwner[senderKey].begin() + utxoIdx);
        
        int inputAmount = inputUTXO.getAmount();
        std::uniform_int_distribution<> percentDist(10, 100);
        int percent = percentDist(gen);
        int sendAmount = (inputAmount * percent) / 100;
        int changeAmount = inputAmount - sendAmount;
        
        std::vector<UTXO> inputs;
        inputs.push_back(inputUTXO);
        
        std::vector<UTXO> outputs;
        outputs.push_back(UTXO("", 0, sendAmount, receiverKey, false));
        
        if (changeAmount > 0) {
            outputs.push_back(UTXO("", 1, changeAmount, senderKey, false));
        }
        
        Transaction tx(senderKey, receiverKey, inputs, outputs);
        transactions.push_back(tx);

        // Print transaction details
        tx.print();
        std::cout << std::endl;

        // Add new outputs to UTXO set
        for (const auto& output : tx.getOutputs()) {
            utxosByOwner[output.getOwnerKey()].push_back(output);
        }
    }

    std::cout << "Generated " << transactions.size() << " transactions" << std::endl;
    return transactions;
}

void saveToFile(const std::vector<Transaction>& transactions, const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }
    
    for (const auto& tx : transactions) {
        file << std::left
             << std::setw(70) << tx.getTransactionId()
             << std::setw(70) << tx.getSender()
             << std::setw(70) << tx.getReceiver()
             << std::setw(20) << tx.getAmount()
             << std::endl;
    }
    
    file.close();
}

int main() {
    auto userPublicKeys = loadUsersFromFile("users.txt");
    auto utxosByOwner = loadUTXOsFromFile("utxos.txt");
    auto transactions = generateTransactions(userPublicKeys, utxosByOwner, TRANSACTION_AMOUNT);
    saveToFile(transactions, "transactions.txt");
    return 0;
}