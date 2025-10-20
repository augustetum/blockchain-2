// src/main.cpp
#include "../include/merkleTree.h"
#include <iostream>
#include <vector>

int main() {
    try {
        // Example data
        std::vector<std::string> data = {
            "Transaction 1",
            "Transaction 2",
            "Transaction 3",
            "Transaction 4",
            "Transaction 5"
        };
        
        std::cout << "Creating Merkle Tree with " << data.size() << " transactions" << std::endl;
        std::cout << std::endl;
        
        // Create Merkle Tree
        MerkleTree tree(data);
        
        // Print root hash
        std::cout << "Root Hash: " << tree.getRootHash() << std::endl;
        std::cout << std::endl;
        
        // Print tree structure
        tree.printTree();
        std::cout << std::endl;
        
        // Generate proof for transaction at index 2
        size_t index = 2;
        std::cout << "Generating proof for transaction at index " << index << ": \"" << data[index] << "\"" << std::endl;
        auto proof = tree.getProof(index);
        
        std::cout << "Proof hashes:" << std::endl;
        for (size_t i = 0; i < proof.size(); i++) {
            std::cout << "  [" << i << "] " << proof[i].substr(0, 16) << "..." << std::endl;
        }
        std::cout << std::endl;
        
        // Verify the proof
        bool isValid = tree.verify(data[index], proof, tree.getRootHash());
        std::cout << "Proof verification: " << (isValid ? "VALID" : "INVALID") << std::endl;
        std::cout << std::endl;
        
        // Try to verify with wrong data
        std::string wrongData = "Transaction 999";
        bool isInvalid = tree.verify(wrongData, proof, tree.getRootHash());
        std::cout << "Verification with wrong data: " << (isInvalid ? "VALID" : "INVALID") << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}