#ifndef POOR_NAME_CHECKER_H
#define POOR_NAME_CHECKER_H

#include <string>
#include <tree_sitter/api.h>

#include "ParsedSource.h"
#include "Detector.h"
#include "NameAnalyzer.h"
#include "FunctionAnalyzer.h"

class PoorNameChecker : public Detector {
    private:
        NameAnalyzer nameAnalyzer;
        FunctionAnalyzer functionAnalyzer;

        // bool isPoorName(const std::string& name, const std::string& type);
        void outputErrorMessage(const std::string& name, const int& line);
        void visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) override;
        // TSNode findIdentifierNode(TSNode node);
        void checkVariableDeclaration(TSNode node, const ParsedSource& parsedSource, int& warningCount);
        void checkFunctionDefinition(TSNode node, const ParsedSource& parsedSource, int& warningCount);
        // std::string extractIdentifierName(const ParsedSource& parsedSource, TSNode identifierNode);
    public:
        PoorNameChecker() {};
        int analyzeSource(const ParsedSource& parsedSource) override;
};

#endif