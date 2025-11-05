#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>
#include "../User.h"
#include "../customGenerator.h"
using std::string;

const int USER_AMOUNT = 1000;

void saveUTXOs(const std::vector<User>& users, const string& filename) {
    std::ofstream file(filename);
    
    for (int i = 0; i < users.size(); i++) {
        file << "0000000000000000000000000000000000000000000000000000000000000000" << " "          // transactionId
             << i << " "                   
             << users[i].getBalance() << " " 
             << users[i].getPublicKey() << " " 
             << "false" << std::endl;          
    }
}

int main() {
    std::ofstream userFile("users.txt");
    std::mt19937 gen(time(NULL)); 
    std::uniform_int_distribution<> distr(100, 1000000);
    std::ostringstream buferis; 
    HashGenerator hasher;
    std::vector<User> users;

    for(int i = 1; i <= USER_AMOUNT; i++){
        
        string name = "Name" + std::to_string(i);
        string publicKey = hasher.generateHash(std::to_string(i)); 
        int balance = distr(gen);
        User user = User(name, publicKey, balance);
        users.push_back(user);
        
        buferis << std::left << std::setw(20) << user.getName() << std::setw(70) << user.getPublicKey() << std::setw(20) << user.getBalance() << std::endl;
    }

    saveUTXOs(users, "utxos.txt");

    userFile << buferis.str();
    userFile.close();
}