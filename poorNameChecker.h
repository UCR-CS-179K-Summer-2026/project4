#ifndef POOR_NAME_CHECKER_H
#define POOR_NAME_CHECKER_H

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "ParsedSource.h"
#include "Detector.h"
#include <tree_sitter/api.h>

struct SuggestedNamesInfo {
    std::vector<std::string> suggestedNames;
    std::string reason;
};

class PoorNameChecker : public Detector {
    private:
        bool isPoorName(const std::string& name, const std::string& type);
        std::vector<std::string> recommendNames(const std::string& name);
        std::unordered_map<std::string, SuggestedNamesInfo> poorNamesMap;
        void outputErrorMessage(const std::string& name, const int& line);
        void visitNode(TSNode node, const ParsedSource& parsedSource, int& warningCount) override;
        TSNode findIdentifierNode(TSNode node);
    public:
        PoorNameChecker();
        int analyzeSource(const ParsedSource& parsedSource) override;
};

#endif