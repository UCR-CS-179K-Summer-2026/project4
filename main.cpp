#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "SmellyCodeDetector.h"

int main() {
    int totalSmellyCount = 0;//counter for any warnings found
    int processedFiles = 0;
    std::string line;
    std::ifstream inputFile("input.txt");

    
    std::cout << "Enter the name(s) of the file(s) to read: ";//asks for input file name, reads input file
    std::getline(std::cin, line);
    inputFile.open(line);

    std::istringstream lineStream(line);
    std::vector<std::string> fileNames;
    std::string fileName;

    while(lineStream >> fileName) {
        fileNames.push_back(fileName);
    }

    if(fileNames.empty()) {
        std::cout << "No file names provided." << std::endl;
        return 1;
    }

    for(const auto& name: fileNames){
        std::ifstream inputFile(name);

         if(!inputFile.is_open()) {//catch for if file fails to open; print error and return
            std::cout << "Error opening file: " << fileName << std::endl;
            continue;
        }


        std::cout << "Analyzing file: " << name << std::endl;

        int smellyCount = SmellyCodeDetector(inputFile).runDetectors(); //run all detectors

        if(smellyCount > 0){
            std::cout << smellyCount << " Smelly code(s) found in the file: " << name << std::endl;
        }

        else{
            std::cout << "No smelly code found in file: " << name << std::endl;
        }

        std::cout << std::endl;

        totalSmellyCount += smellyCount;
        ++processedFiles;
        inputFile.close();

    }

    if(processedFiles == 0){
        std::cout << "No files were processed." << std::endl;
        return 1;
    } 
    
    else {
        std::cout << "Total smelly code(s) found across all files: " << totalSmellyCount << std::endl;
        std::cout << "Total files processed: " << processedFiles << std::endl;
    }
    
    return 0;
}
