#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <vector>
#include "DeclaredVariable.h"
#include "FunctionInfo.h"
#include "ParsedSource.h"

// The Parser class parses the source code from an input file, extracting function information and declared variables
class Parser {
    private:
        std::string buildTypePattern();
        std::string stripComments(const std::string& source);
        std::vector<DeclaredVariable> findDeclarations(const std::string& body);
        std::vector<FunctionInfo> splitIntoFunctionBodies(const std::string& source);
    public:
        ParsedSource parse(std::ifstream& inputFile);
};

#endif