#include <iostream>
#include <fstream>
#include <regex>
#include <string>


int commentChecker(ifstream inputFile){
    int warningCounter = 0;
    int commentCounter = 0;
    std::string line;//string containing current line
    for(int line = 0; std::getline(inputFile, line); line++){//goes through inputfile line by line
        if(std::regex_search(line, )){//if this line contains a function definition
            
        }
    }
}