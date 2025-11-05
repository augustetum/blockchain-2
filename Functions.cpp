#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>
#include <atomic>
#include <omp.h>
#include "User.h"
#include <unordered_map>
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
                                  int txPerBlock,
                                  int difficulty) {
    std::vector<Block> blockchain;
    std::string prevHash = "0000000000000000000000000000000000000000000000000000000000000000";
    int blockNumber = 0;
    const int NUM_THREADS = 5;

    std::cout << "========================================" << std::endl;
    std::cout << "Total transactions to process: " << transactionPool.size() << std::endl;
    std::cout << "Transactions per block: " << txPerBlock << std::endl;
    std::cout << "Difficulty: " << difficulty << " leading zeros" << std::endl;
    std::cout << "Using " << NUM_THREADS << " parallel mining threads" << std::endl;
    std::cout << "========================================\n" << std::endl;

    while (!transactionPool.empty()) {
        blockNumber++;

        int txCount = std::min(txPerBlock, (int)transactionPool.size());
        
        std::cout << "\n--- Block " << blockNumber << " ---" << std::endl;
        std::cout << "Selected " << txCount << " transactions" << std::endl;
        std::cout << "Remaining in pool: " << (transactionPool.size() - txCount) << std::endl;

        std::random_device rd;
        std::mt19937 g(rd());
        
        int totalTxNeeded = std::min(txCount * NUM_THREADS, (int)transactionPool.size());
        std::vector<Transaction> availableTransactions(
            transactionPool.begin(),
            transactionPool.begin() + totalTxNeeded
        );
        std::shuffle(availableTransactions.begin(), availableTransactions.end(), g);

        std::vector<std::vector<Transaction>> threadTxSets(NUM_THREADS);
        for (int i = 0; i < NUM_THREADS; i++) {
            int startIdx = i * txCount;
            int endIdx = std::min(startIdx + txCount, (int)availableTransactions.size());
            
            if (startIdx < availableTransactions.size()) {
                threadTxSets[i] = std::vector<Transaction>(
                    availableTransactions.begin() + startIdx,
                    availableTransactions.begin() + endIdx
                );
            }
        }

        std::atomic<bool> stopFlag(false);
        std::atomic<int> winnerThread(-1);
        std::vector<Block> candidateBlocks;
        
        for (int i = 0; i < NUM_THREADS; i++) {
            if (!threadTxSets[i].empty()) {
                candidateBlocks.push_back(Block(prevHash, difficulty, threadTxSets[i]));
            } else {
                candidateBlocks.push_back(Block(prevHash, difficulty, std::vector<Transaction>()));
            }
        }

        auto startTime = std::chrono::high_resolution_clock::now();

        #pragma omp parallel num_threads(NUM_THREADS)
        {
            int threadId = omp_get_thread_num();
            
            if (!threadTxSets[threadId].empty()) {
                bool success = candidateBlocks[threadId].mineBlockParallel(stopFlag);
                
                if (success) {
                    #pragma omp critical
                    {
                        if (winnerThread == -1) {
                            winnerThread = threadId;
                            stopFlag = true;
                        }
                    }
                }
            }
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        int winner = winnerThread.load();
        if (winner >= 0 && winner < NUM_THREADS) {
            Block& winningBlock = candidateBlocks[winner];
            
            std::cout << "\n>>> Block mined by Thread " << (winner + 1) << " <<<" << std::endl;
            std::cout << "Block hash: " << winningBlock.getBlockHash() << std::endl;
            std::cout << "Merkle root: " << winningBlock.getHeader().getMerkleRoot() << std::endl;
            std::cout << "Nonce found: " << winningBlock.getHeader().getNonce() << std::endl;
            std::cout << "Mining time: " << duration.count() << "ms" << std::endl;

            processTransactions(winningBlock.getTransactions(), users);

            transactionPool.erase(
                transactionPool.begin(),
                transactionPool.begin() + winningBlock.getTransactions().size()
            );

            blockchain.push_back(winningBlock);
            prevHash = winningBlock.getBlockHash();
        }
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