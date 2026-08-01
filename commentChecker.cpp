#include <iostream>
#include <fstream>
#include <regex>
#include <string>


int commentChecker(ifstream& inputFile){
    if (!inputFile.is_open()) {//check if inputFile is reading
        std::cout << "Error: commentChecker could not open file " << std::endl;
        return 0;
    }

    std::string fileContent;//will contain input file as a string
    std::string line;//will hold each line as it gets added to fileContent

    while (std::getline(inputFile, line)){//copy inputFile's contents into fileContent line by line
        fileContent += line + "\n";
    }

    std::regex commentsRegex(R"(//.*?$|/\*.*?\*/)");//translation of comments into regex
    std::regex functionRegex(R"((?:^|\s)[a-zA-Z_][a-zA-Z0-9_<>\s::]*\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)[^{;]*\{)");//translation of functions into regex

}