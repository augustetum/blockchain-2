#include <iostream>
#include <ctime>
using namespace std;

class BlockHeader {
    string prevBlockHash;
    time_t timestamp;
    int version;  
    string merkle_root;
    int nonce;
    int difficulty;

public:
    BlockHeader(string prev, int diff, int ver = 1) 
        : prevBlockHash(prev), 
          difficulty(diff), 
          version(ver),
          nonce(0), 
          merkle_root("") {
        timestamp = time(nullptr);
    }
    
    void setVersion(int v) { version = v; }
    void setMerkleRoot(string mr) { merkle_root = mr; }
    void incrementNonce() { nonce++; }
    void resetNonce() { nonce = 0; }
};