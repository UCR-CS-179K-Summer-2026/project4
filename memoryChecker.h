#ifndef MEMORY_CHECKER_H
#define MEMORY_CHECKER_H

#include <tree_sitter/api.h>
#include <vector>
#include <unordered_map>
#include "Detector.h"
#include "Parser.h"
#include "ParsedSource.h"

class memoryChecker : public Detector {
    private:
        void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;
        std::unordered_map<std::string, int> trackedAllocations;//variable names and line numbers for each memory allocation detected
        void traverse(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings);//traversal helper
        std::string getNode(TSNode node, const std::string& src);
    public: 
        std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) override;
};
#endif