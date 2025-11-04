#include <iostream>
#include "BlockHeader.h"
#include "Transaction.h"
using namespace std;


class Block {
    BlockHeader header;
    vector<Transaction> transactions;
    string blockHash;
};