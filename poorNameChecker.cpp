#include "poorNameChecker.h"
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>

PoorNameChecker::PoorNameChecker() {
    poorNamesMap = {
        {"temp", {{"temporary", "tempVar"}, "Generic temporary variable name"}},
        {"data", {{"info", "dataset"}, "Overly generic name"}},
        {"var", {{"variable", "varName"}, "Too generic and non-descriptive"}},
        {"test", {{"testCase", "unitTest"}, "Non-descriptive test variable name"}},
        {"x", {{"coordinateX", "xValue"}, "Single-letter variable name"}},
        {"y", {{"coordinateY", "yValue"}, "Single-letter variable name"}},
        {"z", {{"coordinateZ", "zValue"}, "Single-letter variable name"}},
    };
}

bool PoorNameChecker::isPoorName(const std::string& name) {
    return poorNamesMap.find(name) != poorNamesMap.end();
}
std::vector<std::string> PoorNameChecker::recommendNames(const std::string& name) {
    auto it = poorNamesMap.find(name);
    if (it != poorNamesMap.end()) {
        return it->second.suggestedNames;
    }
    return {};
}

int PoorNameChecker::analyzeSource(const ParsedSource& parsedSource) {
    std::vector<FunctionInfo> functionBodies = parsedSource.functions;

    int warningCount = 0;
    for (const auto& fb : functionBodies) {
        int startLine = fb.line;
        std::vector<DeclaredVariable> declarations = fb.variables;

        for (const auto& decl : declarations) {
            if(isPoorName(decl.name)) {
                int actualLine = startLine + decl.line - 1;
                std::cout << "Warning: Poor variable name \"" << decl.name
                          << "\" found. Suggested alternatives: ";
                auto suggestions = recommendNames(decl.name);
                for (size_t i = 0; i < suggestions.size(); ++i) {
                    std::cout << suggestions[i];
                    if (i < suggestions.size() - 1) std::cout << ", ";
                }
                std::cout << ". (line " << actualLine << ")\n";
                ++warningCount;
            }
        }
    }

    return warningCount;
}
