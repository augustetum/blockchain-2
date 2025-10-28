#include <iostream>
#include <fstream>
#include <sstream>
#include "User.h"
using std::string;

std::unordered_map<std::string, User> loadUsersFromFile (const std::string& filename) {
    std::unordered_map<std::string, User> userMap; 
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return userMap;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        string name;
        string publicKey;
        int balance;
        
        iss >> name >> publicKey >> balance;
        User user = User(name, publicKey, balance);

        userMap[publicKey] = user;
    }



}