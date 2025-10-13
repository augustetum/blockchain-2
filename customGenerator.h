#ifndef CUSTOMGENERATOR_H
#define CUSTOMGENERATOR_H

#include <string>
#include <cstdint>

class HashGenerator {
private:
    static const int prekesKodai[440];

    // IMPROVEMENT #6: Remove magic numbers - define constants
    static const int LOOKUP_TABLE_SIZE = 440;
    static const int MIXING_ROUNDS = 3;  // IMPROVEMENT #10: Multiple rounds for better avalanche effect
    static const uint64_t MIX_CONSTANT_1 = 0x9E3779B97F4A7C15ULL;  // IMPROVEMENT #3: Stronger mixing constants
    static const uint64_t MIX_CONSTANT_2 = 0x85EBCA6B3C4E9A5DULL;  // Based on golden ratio and random primes

public:
    std::string generateHash(std::string input);
    static uint64_t weightedSum(std::string input);  // IMPROVEMENT #2: Use uint64_t to prevent overflow
    uint64_t varikliukas(uint64_t seed, uint64_t offset);  // IMPROVEMENT #3,4: Return 64-bit for 16 hex output
};

#endif