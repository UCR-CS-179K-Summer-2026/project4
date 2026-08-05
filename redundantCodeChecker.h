#ifndef REDUNDANT_CODE_CHECKER_H
#define REDUNDANT_CODE_CHECKER_H

#include "ParsedSource.h"
#include "Detector.h"

// Detects declared-but-unused local variables within each parsed
// function (redundant/dead code).
class RedundantCodeChecker : public Detector {
    private:
        void visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) override;

        // New helpers for tree-sitter based analysis
        std::string nodeText(TSNode node, const std::string& source);
        std::string extractIdentifierFromDeclarator(TSNode declaratorNode, const std::string& source);
        void collectDeclarations(TSNode declarationNode, const std::string& source, std::vector<std::pair<std::string, TSNode>>& declarations);
        int countIdentifierOccurrences(TSNode scopeNode, const std::string& source, const std::string& name);
        void checkBooleanComparisons(TSNode node, const ParsedSource& parsedSource, int& warningCount);
    public:
        RedundantCodeChecker() = default;
        int analyzeSource(const ParsedSource& parsedSource) override;
};

#endif