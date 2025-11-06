#include <iostream>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <fstream>
#include "Transaction.h"
#include "Block.h"
#include "User.h"
#include "UTXOSet.h"

std::unordered_map<std::string, User> loadUsersFromFile(const std::string& filename);
std::vector<Transaction> loadTransactionsFromFile(const std::string& filename, UTXOSet& utxoSet);
std::vector<Block> mineBlockchain(std::vector<Transaction>& transactionPool,
                                  UTXOSet& utxoSet,
                                  int txPerBlock, int difficulty);
void saveBlockchainToFile(const std::vector<Block>& blockchain, const std::string& filename);

void saveUsersToFile(const std::unordered_map<std::string, User>& users,
                     const UTXOSet& utxoSet,
                     const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    for (const auto& pair : users) {
        const User& user = pair.second;
        // Get updated balance from UTXO set
        int balance = utxoSet.getBalance(user.getPublicKey());
        file << user.getName() << " "
             << user.getPublicKey() << " "
             << balance << std::endl;
    }

    file.close();
    std::cout << "Updated user balances saved to " << filename << std::endl;
}

int main() {
    try {
        std::cout << "========================================" << std::endl;
        std::cout << "   Blockchain v0.1 " << std::endl;
        std::cout << "========================================\n" << std::endl;

        auto users = loadUsersFromFile("users.txt");
        std::cout << "Loaded " << users.size() << " users\n" << std::endl;

        // Load UTXO set from file
        std::cout << "Loading UTXO set..." << std::endl;
        UTXOSet utxoSet;
        utxoSet.loadFromFile("utxos.txt");
        std::cout << std::endl;

        std::cout << "Loading transaction pool..." << std::endl;
        auto transactionPool = loadTransactionsFromFile("transactions.txt", utxoSet);

        if (transactionPool.empty()) {
            std::cerr << "No transactions found. Please run transaction generator first." << std::endl;
            return 1;
        }

        int difficulty;
        std::cout << "Enter mining difficulty: ";
        std::cin >> difficulty;

        auto startTime = std::chrono::high_resolution_clock::now();
        auto blockchain = mineBlockchain(transactionPool, utxoSet, 100, difficulty);
        auto endTime = std::chrono::high_resolution_clock::now();

        auto totalMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        double totalSeconds = totalMilliseconds.count() / 1000.0;

        std::cout << "\n========================================" << std::endl;
        std::cout << "   Blockchain Mining Complete!" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Total blocks mined: " << blockchain.size() << std::endl;
        std::cout << "Total time: " << totalSeconds << " seconds (" << totalMilliseconds.count() << "ms)" << std::endl;
        std::cout << "Transactions remaining in pool: " << transactionPool.size() << std::endl;
        std::cout << "========================================\n" << std::endl;

        saveBlockchainToFile(blockchain, "blockchain.txt");
        saveUsersToFile(users, utxoSet, "users_updated.txt");
        utxoSet.saveToFile("utxo_final.txt");

        std::cout << "\nFirst Block:" << std::endl;
        blockchain[0].print();

        if (blockchain.size() > 1) {
            std::cout << "\nLast Block:" << std::endl;
            blockchain[blockchain.size() - 1].print();
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
