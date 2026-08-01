#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include "commentChecker.h"

int main() {
    int smellyCount = 0;//counter for any warnings found
    std::string fileName;
    std::ifstream inputFile("input.txt");
    
    std::cout << "Enter the name of the file to read: ";//asks for input file name, reads input file
    std::getline(std::cin, fileName);
    inputFile.open(fileName);

    if(!inputFile.is_open()) {//catch for if file fails to open; print error and return
        std::cout << "Error opening file: " << fileName << std::endl;
        return 1;
    }

    smellyCount += commentChecker(inputFile);//makes smellyCount equal to sum of warning from all detector functions(add your functions here)

    if (smellyCount > 0){//smelly code found
        std::cout << smellyCount << " smelly code found in the file." << std::endl;
        return 1;
    }

    else{//no smelly code found
        std::cout << "No smelly code found in the file." << std::endl;
    }


    inputFile.close();


    return 0;
}
