#ifndef REDUNDANT_CODE_CHECKER_H
#define REDUNDANT_CODE_CHECKER_H

#include "ParsedSource.h"
#include "Detector.h"
#include <string>
#include <vector>
#include <utility>

class RedundantCodeChecker : public Detector {
    private:
        // Traversal
        void visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) override;
        
        // Shared helpers
        std::string nodeText(TSNode node, const std::string& source);

         // Check 1: unused/dead variables
        std::string extractIdentifierFromDeclarator(TSNode declaratorNode, const std::string& source);
        void collectDeclarations(TSNode declarationNode, const std::string& source, std::vector<std::pair<std::string, TSNode>>& declarations);
        int countIdentifierOccurrences(TSNode scopeNode, const std::string& source, const std::string& name);
        void checkUnusedVariables(TSNode functionDefNode, const ParsedSource& parsedSource, int& warningCount);

        // Check 2: redundant boolean comparisons (x == true, x != false, etc.)
        void checkBooleanComparison(TSNode binaryExprNode, const ParsedSource& parsedSource, int& warningCount);

        // Check 3: redundant if/else returning boolean literals
        TSNode unwrapToReturnStatement(TSNode node);
        void checkRedundantIfElseReturn(TSNode ifStmtNode, const ParsedSource& parsedSource, int& warningCount);



        // void visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) override;

        // std::string nodeText(TSNode node, const std::string& source);
        // std::string extractIdentifierFromDeclarator(TSNode declaratorNode, const std::string& source);
        // void collectDeclarations(TSNode declarationNode, const std::string& source, std::vector<std::pair<std::string, TSNode>>& declarations);
        
        // // Recursively count identifier nodes matching `name` within scopeNode's subtree.
        // int countIdentifierOccurrences(TSNode scopeNode, const std::string& source, const std::string& name);
        
        // // Detects `x == true`, `x == false`, `x != true`, `x != false` style comparisons,
        // // which should just be `x` or `!x`.
        // void checkBooleanComparisons(TSNode node, const ParsedSource& parsedSource, int& warningCount);

        // // Detects if-else statements where both branches return a value, 
        // // which can be simplified to a single return statement with a conditional expression.
        // void checkRedundantIfElseReturn(TSNode node, const ParsedSource& parsedSource, int& warningCount);
        // TSNode unwrapToReturnStatement(TSNode node);
    public:
        RedundantCodeChecker() = default;
        int analyzeSource(const ParsedSource& parsedSource) override;
};

#endif