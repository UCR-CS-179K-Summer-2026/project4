#include <iostream>
#include <fstream>
#include <string>
#include "redundantCodeChecker.h"
#include "poorNameChecker.h"
#include "Parser.h"

int main() {
    int smellyCount = 0;//counter for any warnings found
    std::string fileName;
    std::ifstream inputFile("input.txt");
    //int commentChecker();//checks if inputFile is commented, returns an int counter for warnings found. Warnings should also be printed to terminal as they are found within the function.
    
    std::cout << "Enter the name of the file to read: ";//asks for input file name, reads input file
    std::getline(std::cin, fileName);
    inputFile.open(fileName);

    if(!inputFile.is_open()) {//catch for if file fails to open; print error and return
        std::cout << "Error opening file: " << fileName << std::endl;
        return 1;
    }

    Parser parser;
    ParsedSource parsedSource = parser.parse(inputFile);

    // All detector functions called here and added to smellyCount
    RedundantCodeChecker redundantCodeChecker;
    PoorNameChecker poorNameChecker;

    //smellyCount = commentChecker(); //comment detector
    smellyCount += redundantCodeChecker.analyzeSource(parsedSource); //redundant code detector
    smellyCount += poorNameChecker.analyzeSource(parsedSource); //poor name detector
    
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