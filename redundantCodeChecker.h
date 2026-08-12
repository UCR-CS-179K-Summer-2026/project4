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

        TSNode findEnclosingScope(TSNode declaratorNode);
        bool blockRedeclares(TSNode blockNode, const std::string& source, const std::string& name);
        
        // Check 2: redundant boolean comparisons (x == true, x != false, etc.)
        void checkBooleanComparison(TSNode binaryExprNode, const ParsedSource& parsedSource, int& warningCount);

        // Check 3: redundant if/else returning boolean literals
        TSNode unwrapToReturnStatement(TSNode node);
        void checkRedundantIfElseReturn(TSNode ifStmtNode, const ParsedSource& parsedSource, int& warningCount);

        // Check 4: redundant if/else if/else statements
        bool alwaysReturns(TSNode statement);
        void checkChainedReturnIfs(TSNode compoundStatementNode, const ParsedSource& parsedSource, int& warningCount);

    public:
        RedundantCodeChecker() = default;
        int analyzeSource(const ParsedSource& parsedSource) override;
};

#endif