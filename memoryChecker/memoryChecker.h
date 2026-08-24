#ifndef MEMORY_CHECKER_H
#define MEMORY_CHECKER_H

#include <tree_sitter/api.h>
#include <vector>
#include <unordered_map>
#include "../ParsedSource.h"
#include "../Detector.h"
#include "../Parser.h"
#include <set>
#include <iostream>
#include <algorithm>

class memoryChecker : public Detector {
    private:
        void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;
        void traverse(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);//traversal helper
        std::string getNode(TSNode node, const std::string& src);
        bool checkIfBodyDeallocates(TSNode node, const std::string& paramName, const std::string& src);
        void collectDeallocatingFunctions(TSNode node, const ParsedSource& parsedSource);
        std::string getAllocationVariable(TSNode node, const std::string& src);//gets variable that corresponds to an allocation
        void analyzeFunction(TSNode functionNode, const ParsedSource& parsedSource, std::vector<Warning>& warnings);
        
        std::unordered_map<std::string, int> trackedAllocations;//variable names and line numbers for each memory allocation detected
        std::unordered_map<std::string, std::set<int>> deallocatingFunctions;//list of deallocating functions

    public: 
        std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) override;
};
#endif