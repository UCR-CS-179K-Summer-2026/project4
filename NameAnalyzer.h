#ifndef NAME_ANALYZER_H
#define NAME_ANALYZER_H

#include "ParsedSource.h"
#include <tree_sitter/api.h>
#include <vector>
#include <Detector.h>
#include <string>

class NameAnalyzer {
    public:
        NameAnalyzer() {};
        std::string extractIdentifierName(const ParsedSource& parsedSource, TSNode identifierNode);
        int getLineNumber(const ParsedSource& parsedSource, TSNode node);
        void outputErrorMessage(const std::string& name, const int& line);
        TSNode findIdentifierNode(TSNode node);
        bool isPoorName(const std::string& name, const std::string& type);
        void outputErrorMessage(const std::string& name, const int& line, std::vector<Warning>& warnings);
};

#endif