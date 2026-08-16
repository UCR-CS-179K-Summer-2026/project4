#ifndef DEAD_CODE_CHECKER_H
#define DEAD_CODE_CHECKER_H

#include "../ParsedSource.h"
#include "../Detector.h"
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <unordered_map>

class DeadCodeChecker : public Detector {
    private:
        void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;
        std::string nodeText(TSNode node, const std::string& source);
        std::string extractIdentifierFromDeclarator(TSNode node, const std::string& source);


        // ---------- Check 5: dead/unused code blocks  ----------
        static TSNode getLastStatement(TSNode compoundStatement);
        static bool alwaysExits(TSNode statement);
        void checkUnreachableCode(TSNode blockNode, const ParsedSource& parsedSource, std::vector<Warning>& warnings);

        // ---------- Check 6: Unused/Dead Functions (unreachable from main) ----------
        std::string getFunctionName(TSNode functionDefNode, const std::string& source);
        void collectCalledFunctionNames(TSNode node, const std::string& source, std::set<std::string>& callees);
        void checkUnusedFunctions(const ParsedSource& parsedSource, std::vector<Warning>& warnings);

    public:
        DeadCodeChecker() = default;
        std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) override;
};

#endif