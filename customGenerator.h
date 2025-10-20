#ifndef CUSTOMGENERATOR_H
#define CUSTOMGENERATOR_H

#include <string>
#include <cstdint>

class HashGenerator {
private:
    static const int prekesKodai[440];
    static const int LOOKUP_TABLE_SIZE = 440;
    static const int MIXING_ROUNDS = 4;  // IMPROVEMENT #1: Increased from 3 to 4 for better avalanche
    
    // IMPROVEMENT #2: Better mixing constants (prime-based, tested for avalanche)
    static const uint64_t MIX_CONSTANT_1 = 0x517CC1B727220A95ULL;
    static const uint64_t MIX_CONSTANT_2 = 0xBF58476D1CE4E5B9ULL;
    static const uint64_t MIX_CONSTANT_3 = 0x94D049BB133111EBULL;
    
    // IMPROVEMENT #3: Add rotation helper for better bit mixing
    inline uint64_t rotl64(uint64_t x, int r) const {
        return (x << r) | (x >> (64 - r));
    }
    
    inline uint64_t rotr64(uint64_t x, int r) const {
        return (x >> r) | (x << (64 - r));
    }

public:
    std::string generateHash(std::string input);
    
    // IMPROVEMENT #4: Enhanced mixing function with rotation
    uint64_t varikliukas(uint64_t seed, uint64_t offset);
    
    // IMPROVEMENT #5: Add secondary mixing for final diffusion
    uint64_t finalMix(uint64_t hash);
};

#endif