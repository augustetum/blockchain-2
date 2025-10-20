#include <iostream>
#include <string>
#include <chrono>
#include "functions.h"
#include "customGenerator.h"

int main(){

    int menuChoice;
                
    std::cout << "Pasirinkite programos eigą:" << std::endl;
    std::cout << "1 | Įvesti norimą hashuoti tekstą ranka" << std::endl;
    std::cout << "2 | Nuskaityti failą" << std::endl;
    std::cout << "3 | Kolizijų paieška" << std::endl;
    std::cout << "4 | Lavinos efekto testas" << std::endl;
    std::cout << "5 | Hiding/puzzle-friendliness testas" << std::endl;
    std::cout << "6 | Hash'avimo efektyvumo testas" << std::endl;
    std::cin >> menuChoice;

    switch(menuChoice){
        case 1: {
            std::cout << "Pasirinkote teksto įvestį ranka" << std::endl;
            std::cout << "---------------------------------" << std::endl;

            std::string hashuojamasTekstas;
            std::cin >> hashuojamasTekstas;

            HashGenerator hash;
            std::string hashed = hash.generateHash(hashuojamasTekstas);
            std::cout << hashed << std::endl;
            break;
        }  

        case 2: {
            std::cout << "Pasirinkote nuskaityti duomenis iš failo" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            HashGenerator hash;
            std::string hashuojamasTekstas = failoNuskaitymas();
            //std::cout<< hashuojamasTekstas;

            auto start = std::chrono::high_resolution_clock::now();
            std::string hashed = hash.generateHash(hashuojamasTekstas);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;

            std::cout << hashed << std::endl;

            std::cout<< "Hashavimas truko: " << diff.count() << " s" <<std::endl;
            break;

        }

        case 3:{
            kolizijos();
            break;
        case 4:
            lavinosEfektas(100000);
            break;
        case 5:
            hidingPuzzleTest();
            break;
        case 6:
            testHashPerformance();
            break;
        }
    }

    return 0;
}