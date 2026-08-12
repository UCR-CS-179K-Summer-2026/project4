#ifndef MEMORY_CHECKER_H
#define MEMORY_CHECKER_H

#include <tree_sitter/api.h>
#include <vector>
#include "Detector.h"
#include "Parser.h"
#include "ParsedSource.h"

class memoryChecker : public Detector {
    private:
        void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;
    public: 
        std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) override;
};
#endif