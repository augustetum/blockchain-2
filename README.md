# Blockchain implementation
 A simplified blockchain implementation using C++ that simulates Proof-of-Work mining, Merkle tree construction and transaction validation.

## Overview
This program depicts a functional blockchain implementation:
-	Generates users  
-	Creates transactions between randomly picked users
-	Mines blocks using PoW consensus 
-	Builds Merkle trees for transaction verification
This implementation uses our previously made LIDL hash, which was enhanced with AI (_Sonnet 4.5_ model) to increase the resistance of collisions, for all cryptographic operations such as users hash, transactions IDs, Merke roots and block hashes, which start with zeros (the number of ‘0’ is based on the user chosen difficulty).

## Features
-	Parallel Proof-of-Work Mining: Multi-threaded mining using OpenMP 
-	Merkle tree implementation _([implemented using this](https://codetrips.com/2020/08/02/modern-c-implementing-a-merkle-tree/))_
-	Transaction Validation: Balance checking and transaction ID verification with UTXO model
-	Custom Hash Function: LIDL hash enhanced for collision resistance
-   Multiple threads compete to mine each block

## Running
### 1.	Clone repository 
`git clone https://github.com/augustetum/blockchain-2.git`
### 2.	Open cloned repository 
`cd blockchain-2`
### 3.	Build everything
`make run`
### 4. Build only main program
`make blockchain`
### 5. Build user generator
`make usergen`
### 6. Build transaction generator
`make txgen`
### 7. To clean *.o *.exe files run
`make clean`
> These Makefile commands work on Windows, MacOS, Linux

## Principle of working 
-	Generating users _(username, public key, balance)_
-	Generating transactions _(sender key, receiver key, amount sent)_
-	Creating blocks with transaction
-	Creation of Merkle root
-	Mining
-	Output to the terminal, transactions.txt and users_updated.txt

## Example of the program
### First block
- Block Hash: 00037ce0935d0260fbba6aa7c04fac494faca9b8a0fdd846191264c78dbc13160
- Previous Hash: 0000000000000000000000000000000000000000000000000000000000000000
- Merkle Root: 6204af027912d2bf984be9b04add70d0a22b98f2645411f1797c40df06664765
- Timestamp: 1762367523
- Difficulty: 3
- Nonce: 4946
- Transactions: 100

### Last block
- Block Hash: 0004ac5fbdb0ea38f8b373a682dd25965ef858b4b89d8a62cf9a3239450f8b5c
- Previous Hash: 000ebe6af32ae36df2b1640defe70f5e647927d1951196d3b739f3dcb5994c37
- Merkle Root: 7684e6bdb2daaa11db603c4814f58df6518d45a5e4ec1f64724c75ac02b83c8f
- Timestamp: 1762367524
- Difficulty: 3
- Nonce: 1208
- Transactions: 100

### Randomly chosen block 98
- Selected 100 transactions
- Remaining in pool: 200
- Mining block...
- Block mined! Nonce: 9523 (after 9524 attempts)
- Block hash: 0009b5b9bfac8baeed8f57ecd889fb4a5f53d091bd50d019ea9f103eb9a58475
- Merkle root: b8c68241d998506cdf4865d0a9d54ad2f7b1fa81517d9ff91a7ae7261fb37c27
- Nonce found: 9523
- Mining time: 35ms


### After the completion 
- Total blocks mined: 100
- Total time: 1 seconds
- Transactions remaining in pool: 0
< blockchain saved to blockchain.txt, updated user balances saved to users_updated.txt

## Parallel mining implementation with OpenMP

Multiple threads simultaneously attempt to mine the same block with different transaction sets, and the first thread to find a valid nonce wins. The implementation of OpenMP was writen with the help of AI _(Sonnet 4.5 model)_

Each thread gets its own candidate block with:
- Same previous hash (maintaining chain)
- Same difficulty target

### Mining Process:
- Five different blocks (for five threads) are generated
- Threads attempt to mine these blocks 
- Each thread check if another thread won _(stopFlag)_ while mining
- Each thread calculate hash with current nonce
- Validation happens by checking leading zeros 
- If valid → return true
- Block (which was mined first) is added to a blockchain
- Other blocks are terminated


## Difficulty impact analysis
| Difficulty | Avg Attempts | Avg Time | Probability |
|-----------|--------------|----------|-------------|
| 1         | ~16          | ~2ms     | 1/16        |
| 2         | ~256         | ~8ms     | 1/256       |
| 3         | ~4096        | ~35ms    | 1/4096      |
| 4         | ~65536       | ~500ms   | 1/65536     |
| 5         | ~1048576     | ~8s      | 1/1048576   |