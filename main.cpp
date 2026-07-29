#include <iostream>
#include <fstream>
#include <string>

int main() {
    int smellyCount = 0;
    std::string fileName;
    std::ifstream inputFile("input.txt");

    std::cout << "Enter the name of the file to read: ";
    std::getline(std::cin, fileName);

    inputFile.open(fileName);

    if(!inputFile.is_open()) {
        std::cout << "Error opening file: " << fileName << std::endl;
        return 1;
    }

    else if (smellyCount == 0){
        std::cout << smellyCount << " smelly code found in the file." << std::endl;
    }

    inputFile.close();


    return 0;
}