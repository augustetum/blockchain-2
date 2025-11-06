#include "UTXOSet.h"
#include "User.h"
#include "Transaction.h"
#include <iostream>
#include <fstream>
#include <sstream>

void UTXOSet::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open UTXO file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string txId, ownerKey, spentStr;
        int outputIndex, amount;

        iss >> txId >> outputIndex >> amount >> ownerKey >> spentStr;

        bool spent = (spentStr == "true");

        // Only load unspent UTXOs
        if (!spent) {
            UTXO utxo(txId, outputIndex, amount, ownerKey, spent);
            std::string utxoId = utxo.getUTXOId();
            utxos[utxoId] = utxo;
        }
    }

    file.close();
    std::cout << "Loaded " << utxos.size() << " unspent UTXOs from " << filename << std::endl;
}

void UTXOSet::addUTXO(const UTXO& utxo) {
    std::string utxoId = utxo.getUTXOId();
    utxos[utxoId] = utxo;
}

bool UTXOSet::removeUTXO(const std::string& utxoId) {
    auto it = utxos.find(utxoId);
    if (it != utxos.end()) {
        utxos.erase(it);
        return true;
    }
    return false;
}

bool UTXOSet::hasUTXO(const std::string& utxoId) const {
    return utxos.find(utxoId) != utxos.end();
}

UTXO UTXOSet::getUTXO(const std::string& utxoId) const {
    auto it = utxos.find(utxoId);
    if (it != utxos.end()) {
        return it->second;
    }
    return UTXO(); // Return empty UTXO if not found
}

std::vector<UTXO> UTXOSet::getUTXOsByOwner(const std::string& ownerKey) const {
    std::vector<UTXO> result;
    for (const auto& pair : utxos) {
        if (pair.second.getOwnerKey() == ownerKey) {
            result.push_back(pair.second);
        }
    }
    return result;
}

int UTXOSet::getBalance(const std::string& ownerKey) const {
    int balance = 0;
    for (const auto& pair : utxos) {
        if (pair.second.getOwnerKey() == ownerKey) {
            balance += pair.second.getAmount();
        }
    }
    return balance;
}

bool UTXOSet::validateInputs(const std::vector<UTXO>& inputs) const {
    for (const auto& input : inputs) {
        std::string utxoId = input.getUTXOId();
        if (!hasUTXO(utxoId)) {
            return false; // UTXO doesn't exist or already spent
        }
    }
    return true;
}

void UTXOSet::applyTransaction(const Transaction& tx) {
    // Remove input UTXOs (mark as spent)
    for (const auto& input : tx.getInputs()) {
        std::string utxoId = input.getUTXOId();
        removeUTXO(utxoId);
    }

    // Add output UTXOs
    for (const auto& output : tx.getOutputs()) {
        addUTXO(output);
    }
}

void UTXOSet::print() const {
    std::cout << "\n========== UTXO Set ==========" << std::endl;
    std::cout << "Total UTXOs: " << utxos.size() << std::endl;

    // Group by owner
    std::unordered_map<std::string, std::vector<UTXO>> byOwner;
    for (const auto& pair : utxos) {
        byOwner[pair.second.getOwnerKey()].push_back(pair.second);
    }

    for (const auto& ownerPair : byOwner) {
        int totalBalance = 0;
        for (const auto& utxo : ownerPair.second) {
            totalBalance += utxo.getAmount();
        }
        std::cout << "Owner: " << ownerPair.first.substr(0, 16) << "... "
                  << "UTXOs: " << ownerPair.second.size()
                  << " Balance: " << totalBalance << std::endl;
    }
    std::cout << "==============================\n" << std::endl;
}

void UTXOSet::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    file << "========== UTXO Set Summary ==========" << std::endl;
    file << "Total UTXOs: " << utxos.size() << std::endl;
    file << "======================================\n" << std::endl;

    // Group by owner
    std::unordered_map<std::string, std::vector<UTXO>> byOwner;
    for (const auto& pair : utxos) {
        byOwner[pair.second.getOwnerKey()].push_back(pair.second);
    }

    // Write summary by owner
    for (const auto& ownerPair : byOwner) {
        int totalBalance = 0;
        for (const auto& utxo : ownerPair.second) {
            totalBalance += utxo.getAmount();
        }
        file << "Owner: " << ownerPair.first.substr(0, 16) << "... "
             << "UTXOs: " << ownerPair.second.size()
             << " Balance: " << totalBalance << std::endl;
    }

    file << "\n======================================" << std::endl;
    file << "Detailed UTXO List:" << std::endl;
    file << "======================================\n" << std::endl;

    // Write detailed UTXO information
    for (const auto& ownerPair : byOwner) {
        file << "\nOwner: " << ownerPair.first << std::endl;
        for (const auto& utxo : ownerPair.second) {
            file << "  UTXO: " << utxo.getUTXOId()
                 << " Amount: " << utxo.getAmount() << std::endl;
        }
    }

    file.close();
    std::cout << "Final UTXO set saved to " << filename << std::endl;
}
