#ifndef DEAD_CODE_CHECKER_H
#define DEAD_CODE_CHECKER_H

#include "ParsedSource.h"
#include "Detector.h"
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <unordered_map>

class DeadCodeChecker : public Detector {
    private:
        // Traversal
        void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;

        // Shared helpers
        std::string nodeText(TSNode node, const std::string& source);

         // Check 1: unused/dead variables
        std::string extractIdentifierFromDeclarator(TSNode declaratorNode, const std::string& source);
        void collectDeclarations(TSNode declarationNode, const std::string& source, std::vector<std::pair<std::string, TSNode>>& declarations);
        int countIdentifierOccurrences(TSNode scopeNode, const std::string& source, const std::string& name);
        void checkUnusedVariables(TSNode functionDefNode, const ParsedSource& parsedSource, std::vector<Warning>& warnings);

        TSNode findEnclosingScope(TSNode declaratorNode);
        bool blockRedeclares(TSNode blockNode, const std::string& source, const std::string& name);
        
        // Check 5: dead/unused code blocks

        static TSNode getLastStatement(TSNode compoundStatement);
        static bool alwaysExits(TSNode statement);
        void checkUnreachableCode(TSNode blockNode, const ParsedSource& parsedSource, std::vector<Warning>& warnings);

        std::string getFunctionName(TSNode functionDefNode, const std::string& source);
        void collectCalledFunctionNames(TSNode node, const std::string& source, std::set<std::string>& callees);
        void checkUnusedFunctions(const ParsedSource& parsedSource, std::vector<Warning>& warnings);

    public:
        DeadCodeChecker() = default;
        std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) override;
};

#endif