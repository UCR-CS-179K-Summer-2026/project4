#ifndef POOR_NAME_CHECKER_H
#define POOR_NAME_CHECKER_H

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

struct SuggestedNamesInfo {
    std::vector<std::string> suggestedNames;
    std::string reason;
};

class PoorNameChecker {
    private:
        bool isPoorName(const std::string& name);
        std::vector<std::string> recommendNames(const std::string& name);
        std::unordered_map<std::string, SuggestedNamesInfo> poorNamesMap;
    public:
        PoorNameChecker();
        int analyzeFile(std::ifstream& inputFile);
};

#endif