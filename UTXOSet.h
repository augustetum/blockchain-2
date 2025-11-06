#ifndef UTXOSET_H
#define UTXOSET_H

#include <unordered_map>
#include <vector>
#include <string>
#include "UTXO.h"

class UTXOSet {
private:
    // Maps UTXO ID (txId:outputIndex) to UTXO object
    std::unordered_map<std::string, UTXO> utxos;

public:
    UTXOSet() = default;

    // Load UTXO set from file
    void loadFromFile(const std::string& filename);

    // Add a UTXO to the set
    void addUTXO(const UTXO& utxo);

    // Remove a UTXO from the set (mark as spent)
    bool removeUTXO(const std::string& utxoId);

    // Check if a UTXO exists and is unspent
    bool hasUTXO(const std::string& utxoId) const;

    // Get a UTXO by its ID
    UTXO getUTXO(const std::string& utxoId) const;

    // Get all UTXOs owned by a specific public key
    std::vector<UTXO> getUTXOsByOwner(const std::string& ownerKey) const;

    // Get total balance for an owner
    int getBalance(const std::string& ownerKey) const;

    // Validate that all inputs exist and are unspent
    bool validateInputs(const std::vector<UTXO>& inputs) const;

    // Apply a transaction: remove inputs, add outputs
    void applyTransaction(const class Transaction& tx);

    // Get the size of the UTXO set
    size_t size() const { return utxos.size(); }

    // Print UTXO set for debugging
    void print() const;

    // Save UTXO set summary to file
    void saveToFile(const std::string& filename) const;
};

#endif
