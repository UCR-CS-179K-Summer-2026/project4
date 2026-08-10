#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <vector>
#include "ParsedSource.h"

// The Parser class parses the source code from an input file, extracting function information and declared variables
class Parser {
    private:
        
    public:
        ParsedSource parse(std::ifstream& inputFile);
};

#endif