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
#include <unordered_set>
#include "customGenerator.h"
#include "Transaction.h"
#include "Block.h"
#include "UTXOSet.h"
using std::string;

// Forward declaration
bool validateTransaction(const Transaction& tx, const UTXOSet& utxoSet);
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

std::vector<Transaction> loadTransactionsFromFile(const std::string& filename, UTXOSet& utxoSet) {
    std::vector<Transaction> transactions;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return transactions;
    }

    // Create a temporary UTXO set to track which UTXOs have been used during loading
    // This prevents the same UTXO from being assigned to multiple transactions
    UTXOSet tempUTXOSet = utxoSet;

    std::string line;
    int skipped = 0;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string txId, sender, receiver;
        int amount;

        iss >> txId >> sender >> receiver >> amount;

        if (!sender.empty() && !receiver.empty() && amount > 0) {
            // Get sender's available UTXOs from the temporary set
            std::vector<UTXO> senderUTXOs = tempUTXOSet.getUTXOsByOwner(sender);

            if (senderUTXOs.empty()) {
                skipped++;
                continue; // Skip if sender has no UTXOs left
            }

            // Find sufficient UTXOs to cover the amount
            std::vector<UTXO> inputs;
            int totalInput = 0;
            for (const auto& utxo : senderUTXOs) {
                inputs.push_back(utxo);
                totalInput += utxo.getAmount();
                if (totalInput >= amount) {
                    break;
                }
            }

            if (totalInput < amount) {
                skipped++;
                continue; // Skip if insufficient funds
            }

            // Create outputs
            std::vector<UTXO> outputs;
            outputs.push_back(UTXO("", 0, amount, receiver, false));

            // Add change output if needed
            int change = totalInput - amount;
            if (change > 0) {
                outputs.push_back(UTXO("", 1, change, sender, false));
            }

            Transaction tx(sender, receiver, inputs, outputs);
            transactions.push_back(tx);

            // Apply this transaction to the temporary UTXO set
            // This marks the input UTXOs as spent so they won't be reused
            tempUTXOSet.applyTransaction(tx);
        }
    }

    file.close();
    std::cout << "Loaded and reconstructed " << transactions.size() << " transactions from file" << std::endl;
    if (skipped > 0) {
        std::cout << "Skipped " << skipped << " transactions due to insufficient UTXOs" << std::endl;
    }
    return transactions;
}

bool processTransactions(const std::vector<Transaction>& transactions,
                         UTXOSet& utxoSet) {
    for (const auto& tx : transactions) {
        // Apply the transaction to the UTXO set
        // This removes input UTXOs and adds output UTXOs
        utxoSet.applyTransaction(tx);
    }
    return true;
}

std::vector<Block> mineBlockchain(std::vector<Transaction>& transactionPool,
                                  UTXOSet& utxoSet,
                                  int txPerBlock,
                                  int difficulty) {
    std::vector<Block> blockchain;
    std::string prevHash = "0000000000000000000000000000000000000000000000000000000000000000";
    int blockNumber = 0;
    const int NUM_THREADS = 5;
    const int ATTEMPT_INCREMENT = 1000;

    std::cout << "========================================" << std::endl;
    std::cout << "Total transactions to process: " << transactionPool.size() << std::endl;
    std::cout << "Transactions per block: " << txPerBlock << std::endl;
    std::cout << "Difficulty: " << difficulty << " leading zeros" << std::endl;
    std::cout << "Using " << NUM_THREADS << " parallel mining threads" << std::endl;
    std::cout << "Attempt limit per round: " << ATTEMPT_INCREMENT << std::endl;
    std::cout << "========================================\n" << std::endl;

    while (!transactionPool.empty()) {
        blockNumber++;

        std::cout << "\n--- Block " << blockNumber << " ---" << std::endl;
        std::cout << "Transaction pool size: " << transactionPool.size() << std::endl;

        // Validate transactions and select valid ones for this block
        std::vector<Transaction> validTransactions;
        UTXOSet tempUTXOSet = utxoSet; // Create a temporary copy for validation

        for (const auto& tx : transactionPool) {
            if (validateTransaction(tx, tempUTXOSet)) {
                validTransactions.push_back(tx);
                // Temporarily apply this transaction to avoid double-spending within the same block
                tempUTXOSet.applyTransaction(tx);

                if (validTransactions.size() >= txPerBlock) {
                    break; // We have enough valid transactions for this block
                }
            }
        }

        if (validTransactions.empty()) {
            std::cout << "No valid transactions left in pool. Stopping." << std::endl;
            break;
        }

        int txCount = validTransactions.size();
        std::cout << "Selected " << txCount << " valid transactions" << std::endl;
        std::cout << "Remaining in pool: " << (transactionPool.size() - txCount) << std::endl;

        // For parallel mining, all threads will mine the SAME set of transactions
        // This ensures consistency and avoids UTXO conflicts
        std::vector<std::vector<Transaction>> threadTxSets(NUM_THREADS);
        for (int i = 0; i < NUM_THREADS; i++) {
            threadTxSets[i] = validTransactions; // All threads get the same valid transactions
        }
        std::vector<Block> candidateBlocks;
        for (int i = 0; i < NUM_THREADS; i++) {
            if (!threadTxSets[i].empty()) {
                candidateBlocks.push_back(Block(prevHash, difficulty, threadTxSets[i]));
            } else {
                candidateBlocks.push_back(Block(prevHash, difficulty, std::vector<Transaction>()));
            }
        }
        bool blockMined = false;
        int currentAttemptLimit = ATTEMPT_INCREMENT;
        int retryCount = 0;
        std::atomic<int> winnerThread(-1); 
        auto startTime = std::chrono::high_resolution_clock::now();

        while (!blockMined) {
            std::atomic<bool> stopFlag(false);
            #pragma omp parallel num_threads(NUM_THREADS)
            {
                int threadId = omp_get_thread_num();
                
                if (!threadTxSets[threadId].empty()) {
                    bool success = candidateBlocks[threadId].mineBlockParallel(stopFlag, ATTEMPT_INCREMENT);
                    
                    if (success) {
                        #pragma omp critical
                        {
                            if (winnerThread == -1) {
                                winnerThread = threadId;
                                stopFlag = true;
                                blockMined = true;
                            }
                        }
                    }
                }
            }

            if (blockMined) {
                break; 
            }

            retryCount++;
            currentAttemptLimit += ATTEMPT_INCREMENT;
            
            std::cout << "Failed to mine block with " << (currentAttemptLimit - ATTEMPT_INCREMENT) 
                      << " attempts per thread. Retrying with " << currentAttemptLimit 
                      << " attempts per thread..." << std::endl;
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        int totalAttempts = 0;
        for (int i = 0; i < NUM_THREADS; i++) {
            if (!threadTxSets[i].empty()) {
                totalAttempts += candidateBlocks[i].getAttempts();
            }
        }

        int winner = winnerThread.load();
        if (winner >= 0 && winner < NUM_THREADS) {
            Block& winningBlock = candidateBlocks[winner];
            
            std::cout << "Block mined by Thread " << (winner + 1) << std::endl;
            
            std::cout << "Block hash: " << winningBlock.getBlockHash() << std::endl;
            std::cout << "Merkle root: " << winningBlock.getHeader().getMerkleRoot() << std::endl;
            std::cout << "Nonce found: " << winningBlock.getHeader().getNonce() << std::endl;
            std::cout << "Winner's attempts: " << winningBlock.getAttempts() << std::endl;
            std::cout << "Total attempts (all 5 threads): " << totalAttempts << std::endl;
            std::cout << "Mining time: " << duration.count() << "ms" << std::endl;

            // Apply the winning block's transactions to the UTXO set
            processTransactions(winningBlock.getTransactions(), utxoSet);

            // Remove the mined transactions from the pool
            // We need to remove the exact transactions that were mined
            std::vector<Transaction> remainingPool;
            std::unordered_set<std::string> minedTxIds;

            for (const auto& tx : winningBlock.getTransactions()) {
                minedTxIds.insert(tx.getTransactionId());
            }

            for (const auto& tx : transactionPool) {
                if (minedTxIds.find(tx.getTransactionId()) == minedTxIds.end()) {
                    remainingPool.push_back(tx);
                }
            }

            transactionPool = remainingPool;

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


bool validateTransaction(const Transaction& tx, const UTXOSet& utxoSet) {
    // Validate that all input UTXOs exist and are unspent
    const auto& inputs = tx.getInputs();
    const auto& outputs = tx.getOutputs();

    if (inputs.empty()) {
        std::cerr << "Invalid transaction: no inputs" << std::endl;
        return false;
    }

    // Check that all input UTXOs exist in the UTXO set
    if (!utxoSet.validateInputs(inputs)) {
        std::cerr << "Invalid transaction: one or more input UTXOs don't exist or already spent" << std::endl;
        return false;
    }

    // Verify input ownership - all inputs must belong to the sender
    std::string sender = tx.getSender();
    for (const auto& input : inputs) {
        if (input.getOwnerKey() != sender) {
            std::cerr << "Invalid transaction: input UTXO doesn't belong to sender" << std::endl;
            return false;
        }
    }

    // Calculate total input and output amounts
    int totalInput = 0;
    for (const auto& input : inputs) {
        totalInput += input.getAmount();
    }

    int totalOutput = 0;
    for (const auto& output : outputs) {
        totalOutput += output.getAmount();
    }

    // Verify input amount equals output amount (no inflation)
    if (totalInput != totalOutput) {
        std::cerr << "Invalid transaction: input amount (" << totalInput
                  << ") doesn't match output amount (" << totalOutput << ")" << std::endl;
        return false;
    }

    return true;
}