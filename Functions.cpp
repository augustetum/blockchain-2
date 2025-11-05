#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include "User.h"
#include <unordered_map>
#include "customGenerator.h"
#include "Transaction.h"
#include "Block.h"
using std::string;

std::unordered_map<std::string, User> loadUsersFromFile (const std::string& filename) {
    std::unordered_map<std::string, User> userMap; 
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return userMap;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        string name;
        string publicKey;
        int balance;
        
        iss >> name >> publicKey >> balance;
        User user = User(name, publicKey, balance);

        userMap[publicKey] = user;
    }

    file.close();
    return userMap;
}

std::vector<Transaction> loadTransactionsFromFile(const std::string& filename) {
    std::vector<Transaction> transactions;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return transactions;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string sender, receiver;
        double amount;

        iss >> sender >> receiver >> amount;

        if (!sender.empty() && !receiver.empty() && amount > 0) {
            Transaction tx(sender, receiver, amount);
            transactions.push_back(tx);
        }
    }

    file.close();
    std::cout << "Loaded " << transactions.size() << " transactions from file" << std::endl;
    return transactions;
}

bool processTransactions(const std::vector<Transaction>& transactions,
                         std::unordered_map<std::string, User>& users) {
    for (const auto& tx : transactions) {
        std::string sender = tx.getSender();
        std::string receiver = tx.getReceiver();
        double amount = tx.getAmount();

        users[sender].setBalance(users[sender].getBalance() - amount);
        users[receiver].setBalance(users[receiver].getBalance() + amount);
    }

    return true;
}

std::vector<Block> mineBlockchain(std::vector<Transaction>& transactionPool,
                                  std::unordered_map<std::string, User>& users,
                                  int txPerBlock = 100,
                                  int difficulty = 3) {
    std::vector<Block> blockchain;
    std::string prevHash = "0000000000000000000000000000000000000000000000000000000000000000"; //genesis hashas
    int blockNumber = 0;

    std::cout << "========================================" << std::endl;
    std::cout << "Total transactions to process: " << transactionPool.size() << std::endl;
    std::cout << "Transactions per block: " << txPerBlock << std::endl;
    std::cout << "Difficulty: " << difficulty << " leading zeros" << std::endl;
    std::cout << "========================================\n" << std::endl;

    while (!transactionPool.empty()) {
        blockNumber++;

        int txCount = std::min(txPerBlock, (int)transactionPool.size());
        std::vector<Transaction> blockTransactions(
            transactionPool.begin(),
            transactionPool.begin() + txCount
        );

        std::cout << "\n--- Block " << blockNumber << " ---" << std::endl;
        std::cout << "Selected " << txCount << " transactions" << std::endl;
        std::cout << "Remaining in pool: " << (transactionPool.size() - txCount) << std::endl;

        auto startTime = std::chrono::high_resolution_clock::now();
        Block block(prevHash, difficulty, blockTransactions);
        auto endTime = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        std::cout << "Block hash: " << block.getBlockHash() << std::endl;
        std::cout << "Merkle root: " << block.getHeader().getMerkleRoot() << std::endl;
        std::cout << "Nonce found: " << block.getHeader().getNonce() << std::endl;
        std::cout << "Mining time: " << duration.count() << "ms" << std::endl;

        processTransactions(blockTransactions, users);

        transactionPool.erase(transactionPool.begin(), transactionPool.begin() + txCount);

        blockchain.push_back(block);
        prevHash = block.getBlockHash();
    }

    return blockchain;
}

void saveBlockchainToFile(const std::vector<Block>& blockchain, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    for (size_t i = 0; i < blockchain.size(); i++) {
        const Block& block = blockchain[i];
        file << "Block " << (i + 1) << std::endl;
        file << "Block Hash: " << block.getBlockHash() << std::endl;
        file << "Previous Hash: " << block.getHeader().getPrevBlockHash() << std::endl;
        file << "Merkle Root: " << block.getHeader().getMerkleRoot() << std::endl;
        file << "Timestamp: " << block.getHeader().getTimestamp() << std::endl;
        file << "Difficulty: " << block.getHeader().getDifficulty() << std::endl;
        file << "Nonce: " << block.getHeader().getNonce() << std::endl;
        file << "Transactions: " << block.getTransactions().size() << std::endl;
        file << "---" << std::endl;
    }

    file.close();
    std::cout << "\nBlockchain saved to " << filename << std::endl;
}


bool validateTransaction(const Transaction& tx, const std::unordered_map<std::string, User>& users) {
    auto senderIt = users.find(tx.getSender());
    if (senderIt == users.end()) {
        std::cerr << "Invalid transaction: sender not found" << std::endl;
        return false;
    }
    
    if (senderIt->second.getBalance() < tx.getAmount()) {
        std::cerr << "Invalid transaction: insufficient balance. Sender has " << senderIt->second.getBalance() << " but trying to send " << tx.getAmount() << std::endl;
        return false;
    }
    
    HashGenerator hasher;
    std::string expectedId = hasher.generateHash(
        tx.getSender() + tx.getReceiver() + std::to_string(tx.getAmount())
    );
    
    if (tx.getTransactionId() != expectedId) {
        std::cerr << "Invalid transaction: transaction ID mismatch" << std::endl;
        return false;
    }
    
    return true;
}