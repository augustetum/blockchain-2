#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>
#include "BlockHeader.h"
#include "Transaction.h"
#include "include/merkleTree.h"
using namespace std;

class Block {
private:
    BlockHeader header;
    vector<Transaction> transactions;
    string blockHash;

public:
    Block(const string& prevHash, int difficulty, const vector<Transaction>& txs);

    BlockHeader getHeader() const { return header; }
    vector<Transaction> getTransactions() const { return transactions; }
    string getBlockHash() const { return blockHash; }


    void buildMerkleTree();
    string calculateBlockHash();
    void mineBlock();
    bool isHashValid(const string& hash, int difficulty) const;
    void print() const;
};

#endif