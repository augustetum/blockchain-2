#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <filesystem>
#include <chrono>
#include <random>
#include <unordered_map>
#include <utility>
#include <algorithm> 
#include <iomanip>
#include "functions.h"
#include "customGenerator.h"

/* for MAC

std::string pasirinktiFaila(){
    while(true){
        try{
            //system("ls *.txt ../failaiHashavimui/*.txt 2>/dev/null > temp.txt");    
            system("for /r %A in (*.txt) do @echo %~nxA >> temp.txt"); //windowsam
            std::ifstream tempFail("temp.txt");
            std::vector<std::string> failuPav;
     
            std::string failoPav;
            while(std::getline(tempFail, failoPav)){
                failuPav.push_back(failoPav);

            }
            tempFail.close();
            //system("rm temp.txt"); 
            system("del temp.txt");
            std::string fail;

            std::cout << "Pasirinkite norimą failą" << std::endl;
            for(std::size_t i = 0; i < failuPav.size(); ++i){
                std::cout << (i + 1) << "| " << failuPav[i] << std::endl;
            }
            int choice;
            std::cin >> choice;
            if (choice < 1 || choice > static_cast<int>(failuPav.size())) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("Neteisingas failo pasirinkimas");
            } else {
                 fail = failuPav[static_cast<std::size_t>(choice - 1)];
            }
            return fail;
            break;
        } catch (const std::runtime_error &e) {
            std::cout << e.what() << std::endl;
            continue;
        } catch (const char* e){
            std::cout << e << std::endl;
            throw;
        }
    }
}*/


std::string failoNuskaitymas(){
    std::string fileName = pasirinktiFaila();
    std::ifstream f(fileName);
    std::string tekstas;
    std::string eilute;
    while (getline(f, eilute))
    {
        tekstas += eilute;
        if (!f.eof()) { //kad neimtu newlines
            tekstas += '\n';
        }
    }
    f.close();
    return tekstas;
}


namespace fs = std::filesystem;
std::string pasirinktiFaila(){
    while(true){
        try{
            fs::path folderPath = "failaiHashavimui";
            if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
                throw std::runtime_error("failaiHashavimui nerastas");
            }
            
            std::vector<std::string> failuPav;
            for (const auto& entry : fs::directory_iterator(folderPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                    failuPav.push_back(entry.path().filename().string());
                }
            }
            if (failuPav.empty()) {
                throw std::runtime_error("failaiHashavimui nera .txt failų");
            }
            std::cout << "Pasirinkite norimą failą:" << std::endl;
            for(std::size_t i = 0; i < failuPav.size(); ++i){
                std::cout << (i + 1) << "| " << failuPav[i] << std::endl;
            }
            
            int choice;
            std::cin >> choice;
            
            if (std::cin.fail() || choice < 1 || choice > static_cast<int>(failuPav.size())) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("Neteisingas failo pasirinkimas");
            }
            
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::string selectedFile = (folderPath / failuPav[static_cast<std::size_t>(choice - 1)]).string();
            return selectedFile;
            
        } catch (const std::runtime_error &e) {
            std::cout << e.what() << std::endl;
            continue;
        } catch (const std::exception &e) {
            std::cout << "Klaida: " << e.what() << std::endl;
            throw;
        }
    }
}

// Funkcija atsitiktiniam string generavimui
std::string stringGeneratorius(int length, std::mt19937& gen) {
    static const char charset[] = 
        "abcdefghijklmnopqrstuvwxyząčęėįšųū"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZĄČĘĖĮŠŲŪ"
        "0123456789";
    static const int charsetSize = sizeof(charset) - 1;
    std::uniform_int_distribution<> dist(0, charsetSize - 1);
    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += charset[dist(gen)];
    }
    return result;
}

//Bitų skirtumas (hex string → bit skirtumai)
int countBitDifference(const std::string& str1, const std::string& str2) {
    int diff = 0;
    for (size_t i = 0; i < str1.size() && i < str2.size(); ++i) {
        // Konvertuojame hex char į 4-bit reikšmę
        auto hexToBits = [](char c) -> int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return 0;
        };

        int val1 = hexToBits(str1[i]);
        int val2 = hexToBits(str2[i]);
        int xorResult = val1 ^ val2;

        // Skaičiuojame, kiek bitų skiriasi (4 bitai per hex char)
        for (int bit = 0; bit < 4; ++bit) {
            if (xorResult & (1 << bit)) {
                diff++;
            }
        }
    }
    return diff;
}

//Hex skirtumas
int countHexDifference(const std::string& str1, const std::string& str2) {
    int diff = 0;
    for (size_t i = 0; i < str1.size() && i < str2.size(); ++i) {
        if (str1[i] != str2[i]) {
            diff++;
        }
    }
    return diff;
}

//Lavinos efekto testas
void lavinosEfektas(int numPairs) {
    HashGenerator hashGenerator;
    std::random_device rd;
    std::mt19937 gen(rd());
    const int STRING_LENGTH = 64;  
    std::uniform_int_distribution<> pos_dist(0, STRING_LENGTH - 1);
    std::uniform_int_distribution<int> char_dist('a', 'z'); 

    int totalBitDiff = 0, totalHexDiff = 0;
    int minBitDiff = std::numeric_limits<int>::max(), maxBitDiff = 0;
    int minHexDiff = std::numeric_limits<int>::max(), maxHexDiff = 0;
    int zeroBitDiffs = 0, zeroHexDiffs = 0;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numPairs; ++i) {
        std::string original;
        original.reserve(STRING_LENGTH);
        for (int j = 0; j < STRING_LENGTH; ++j) {
            original += static_cast<char>(char_dist(gen));
        }
        

        std::string modified = original;
        int pos = pos_dist(gen);
        char newChar;
        do {
            newChar = static_cast<char>(char_dist(gen));
        } while (newChar == original[pos]);
        modified[pos] = newChar;

        std::string hash1 = hashGenerator.generateHash(original);
        std::string hash2 = hashGenerator.generateHash(modified);
        if (hash1.length() != hash2.length()) {
            std::cerr << "Klaida: Skirtingi hash ilgiai! " << hash1.length() << " vs " << hash2.length() << std::endl;
            continue;
        }

        int bitDiff = countBitDifference(hash1, hash2);
        int hexDiff = countHexDifference(hash1, hash2);


        if (bitDiff == 0) zeroBitDiffs++;
        if (hexDiff == 0) zeroHexDiffs++;


        totalBitDiff += bitDiff;
        totalHexDiff += hexDiff;
        minBitDiff = std::min(minBitDiff, bitDiff);
        maxBitDiff = std::max(maxBitDiff, bitDiff);
        minHexDiff = std::min(minHexDiff, hexDiff);
        maxHexDiff = std::max(maxHexDiff, hexDiff);


        if ((i + 1) % 10000 == 0 || i == 0 || i == numPairs - 1) {
            std::cout << "Apdorota " << (i + 1) << " porų..." << std::endl;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;

    size_t hashLength = hashGenerator.generateHash("test").length(); // 8 hex simboliai
    size_t hashBitLength = hashLength * 4; // 8 hex * 4 bits = 32 bitai

    double avgBitDiff = static_cast<double>(totalBitDiff) / numPairs;
    double avgHexDiff = static_cast<double>(totalHexDiff) / numPairs;
    double bitDiffPercentage = (avgBitDiff / hashBitLength) * 100.0;
    double hexDiffPercentage = (avgHexDiff / hashLength) * 100.0;
    
    std::cout << "\n=== Lavinos efekto testavimo rezultatai ===" << std::endl;
    std::cout << "Iš viso išbandyta porų: " << numPairs << std::endl;
    std::cout << "Testo trukmė: " << std::fixed << std::setprecision(2) << duration.count() << " s" << std::endl;
    std::cout << "Hash'o dydis: " << hashLength << " hex simboliai (" << hashBitLength << " bitai)" << std::endl;

    std::cout << "\n--- Bitų lygmenyje ---" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Minimalus skirtumas: " << minBitDiff << "/" << hashBitLength << " bitų ("
              << (minBitDiff * 100.0 / hashBitLength) << "%)" << std::endl;
    std::cout << "  Maksimalus skirtumas: " << maxBitDiff << "/" << hashBitLength << " bitų ("
              << (maxBitDiff * 100.0 / hashBitLength) << "%)" << std::endl;
    std::cout << "  Vidutinis skirtumas: " << std::setprecision(2) << avgBitDiff << " bitų ("
              << bitDiffPercentage << "%)" << std::endl;
    std::cout << "  Idealus lavinos efektas: 50% (16.00 bitų)" << std::endl;
    std::cout << "  Kolizijos (0 bitų skirtumas): " << zeroBitDiffs << " ("
              << std::setprecision(4) << (zeroBitDiffs * 100.0 / numPairs) << "%)" << std::endl;

    std::cout << "\n--- Hex simbolių lygmenyje ---" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Minimalus skirtumas: " << minHexDiff << "/" << hashLength << " simbolių ("
              << (minHexDiff * 100.0 / hashLength) << "%)" << std::endl;
    std::cout << "  Maksimalus skirtumas: " << maxHexDiff << "/" << hashLength << " simbolių ("
              << (maxHexDiff * 100.0 / hashLength) << "%)" << std::endl;
    std::cout << "  Vidutinis skirtumas: " << std::setprecision(2) << avgHexDiff << " simbolių ("
              << hexDiffPercentage << "%)" << std::endl;
    std::cout << "  Kolizijos (0 simbolių skirtumas): " << zeroHexDiffs << " ("
              << std::setprecision(4) << (zeroHexDiffs * 100.0 / numPairs) << "%)" << std::endl;

    std::cout << "\n=== Testas baigtas ===" << std::endl;
}

void kolizijos() {
    int numPairs = 1000000;
    int length = 10;
    std::cout << "String vienos poros simbolių skaičius: " << length << std::endl;
    
    HashGenerator hash;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    int pairCollisions = 0;
    for (int i = 0; i < numPairs; ++i) {
        std::string str1 = stringGeneratorius(length, gen);
        std::string str2 = stringGeneratorius(length, gen);
        
        std::string hash1 = hash.generateHash(str1);
        std::string hash2 = hash.generateHash(str2);
        
        if (hash1 == hash2) {
            ++pairCollisions;
        }
    }

    std::cout << "Porų skaičius: " << numPairs << std::endl;
    std::cout << "Porų kolizijų skaičius: " << pairCollisions << std::endl;
    double perPairRate = static_cast<double>(pairCollisions) / numPairs * 100.0;
    std::cout << "Porų kolizijų tikimybė: " << std::fixed << std::setprecision(6)
              << perPairRate << " %" << std::endl;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = endTime - startTime;
    std::cout << "Laikas: " << diff.count() << " s" << std::endl;
}

void hidingPuzzleTest() {
    HashGenerator hashGenerator;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::cout << "=== Hiding savybės testas ===" << std::endl;
    
    std::string salt = stringGeneratorius(32, gen);
    std::string secret = "Slapta žinutė";
    
    std::cout << "   Salt (r): " << salt << std::endl;
    std::cout << "   Secret (x): " << secret << std::endl;
    
    std::string hash = hashGenerator.generateHash(salt + secret);
    std::cout << "   H(r || x): " << hash << std::endl;
    
    
    std::cout << "\n=== Puzzle-Friendliness testas ===" << std::endl;
    std::cout << "Ieškoma x' tokio, kad H(r || x') prasideda su '0'..." << std::endl;
    
    const int NUM_TRIALS = 10;  
    const std::string TARGET_PREFIX = "0";
    const double EXPECTED_ATTEMPTS = 16.0;  // 16^1 = 16 (vienas hex simbolis)
    
    std::vector<int> all_attempts;
    
    for (int trial = 0; trial < NUM_TRIALS; trial++) {
        int attempts = 0;
        std::string x_prime;
        std::string target_hash;
        
        while (true) {
            x_prime = stringGeneratorius(10, gen);
            target_hash = hashGenerator.generateHash(salt + x_prime);
            attempts++;
            
            if (target_hash.substr(0, TARGET_PREFIX.length()) == TARGET_PREFIX) {
                all_attempts.push_back(attempts);
                break;
            }
        }
        
        std::cout << "   Bandymas #" << (trial + 1) << ": x' rastas po " 
                  << attempts << " bandymų (x' = " << x_prime 
                  << ", H = " << target_hash << ")" << std::endl;
    }
    
    //Statistika
    double mean = std::accumulate(all_attempts.begin(), all_attempts.end(), 0.0) / NUM_TRIALS;
    double variance = 0.0;
    for (int a : all_attempts) {
        variance += (a - mean) * (a - mean);
    }
    variance /= NUM_TRIALS;
    double std_dev = std::sqrt(variance);
    
    std::cout << "\n   Statistika per " << NUM_TRIALS << " bandymų:" << std::endl;
    std::cout << "   - Tikėtina bandymų (16^1): ~" << EXPECTED_ATTEMPTS << std::endl;
    std::cout << "   - Faktinis vidurkis: " << std::fixed << std::setprecision(2) << mean << std::endl;
    std::cout << "   - Standartinis nuokrypis: " << std_dev << std::endl;
    std::cout << "   - Mažiausiai bandymų: " << *std::min_element(all_attempts.begin(), all_attempts.end()) << std::endl;
    std::cout << "   - Daugiausiai bandymų: " << *std::max_element(all_attempts.begin(), all_attempts.end()) << std::endl;
    
    double ratio = mean / EXPECTED_ATTEMPTS;
    std::cout << "   - Santykis (faktinis/tikėtinas): " << ratio << std::endl;
    
    bool passes = std::abs(mean - EXPECTED_ATTEMPTS) < (2 * std_dev / std::sqrt(NUM_TRIALS));
    std::cout << "\n   Testas " << (passes ? "PAVYKO" : "NEPAVYKO") << std::endl;
}

void testHashPerformance() {
    const std::string filename = "../failaiHashavimui/konstitucija.txt";
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Nepavyko atidaryti failo: " << filename << std::endl;
        return;
    }
    
    // Read all lines from the file
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    file.close();
    
    if (lines.empty()) {
        std::cerr << "Failas tusčias arba neturi eilučių" << std::endl;
        return;
    }
    
    HashGenerator hashGenerator;
    
    std::cout << "=== Hash'avimo efektyvumo testas ===" << std::endl;
    std::cout << "Failas: " << filename << std::endl;
    std::cout << "Iš viso eilučių: " << lines.size() << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << std::setw(10) << "Eilučių" << std::setw(20) << "Laikas (s)" 
              << std::setw(20) << "Vid. laikas (ms)" << std::setw(15) << "Hash'as" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (int chunkSize = 1; chunkSize <= lines.size(); ) {
        std::string input;
        for (int i = 0; i < chunkSize; ++i) {
            input += lines[i] + "\n";
        }

        hashGenerator.generateHash("apsilimas");

        auto start = std::chrono::high_resolution_clock::now();
        std::string hash = hashGenerator.generateHash(input);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration = end - start;
        double avgTimePerHash = duration.count() * 1000.0; // Convert to milliseconds

        std::cout << std::setw(10) << chunkSize
                  << std::setw(20) << std::scientific << std::setprecision(6) << duration.count()
                  << std::setw(20) << std::fixed << std::setprecision(6) << avgTimePerHash
                  << std::setw(15) << hash.substr(0, 8) << "..." << std::endl;

        if (chunkSize >= lines.size()) {
            break;
        }

        chunkSize *= 2;
        if (chunkSize > lines.size()) {
            chunkSize = lines.size();
        }
    }
    
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "Testas baigtas" << std::endl;
}