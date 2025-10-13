#pragma once

#include <string>
#include <random>
#include <vector>

std::string pasirinktiFaila();
std::string failoNuskaitymas();
std::string stringGeneratorius(int length, std::mt19937& gen);
void kolizijos();
void lavinosEfektas(int numPairs = 100000);
void hidingPuzzleTest();
void testHashPerformance();