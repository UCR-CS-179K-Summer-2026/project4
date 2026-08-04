#include <iostream>
#include <fstream>
#include <string>
#include "SmellyCodeDetector.h"

int main() {
    int smellyCount = 0;//counter for any warnings found
    std::string fileName;
    std::ifstream inputFile;
    
    std::cout << "Enter the name of the file to read: ";//asks for input file name, reads input file
    std::getline(std::cin, fileName);
    inputFile.open(fileName);

    if(!inputFile.is_open()) {//catch for if file fails to open; print error and return
        std::cout << "Error opening file: " << fileName << std::endl;
        return 1;
    }

    // // All detector functions called here and added to smellyCount
    smellyCount += SmellyCodeDetector(inputFile).runDetectors(); //run all detectors
    
    if (smellyCount > 0){ //smelly code found
        std::cout << smellyCount << " Smelly code found in the file." << std::endl;
        return 1;
    }

    else{ //no smelly code found (smellyCount = 0)
        std::cout << "No smelly code found in the file." << std::endl;
    }

    inputFile.close();

    return 0;
}
