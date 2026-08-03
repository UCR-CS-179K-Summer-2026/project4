#include "poorNameChecker.h"
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>

PoorNameChecker::PoorNameChecker() {
    poorNamesMap = {
        {"temp", {{"currentValue", "temporaryValue"}, "Generic temporary variable name"}},
        {"data", {{"userData", "apiData"}, "Does not describe what kind of data it holds"}},
        {"var", {{"variable", "varName"}, "Does not describe the purpose of the variable"}},
        {"test", {{"testCase", "unitTest"}, "Does not describe what the test is for"}},
        {"x", {{"coordinateX", "xValue"}, "Single-letter variable name"}},
        {"y", {{"coordinateY", "yValue"}, "Single-letter variable name"}},
        {"z", {{"coordinateZ", "zValue"}, "Single-letter variable name"}},
        {"flag", {{"isTrue", "isSet"}, "Does not describe the purpose of the flag"}},
        {"m", {{"rows", "maxValue"}, "Non-descriptive variable name"}},
        {"n", {{"columns", "count"}, "Non-descriptive variable name"}},
    };
}

bool PoorNameChecker::isPoorName(const std::string& name) {
    if(name.length() < 3 && name != "id" && name != "i" && name != "j" && name != "k") {
        return true;
    }
    return poorNamesMap.find(name) != poorNamesMap.end();
}
std::vector<std::string> PoorNameChecker::recommendNames(const std::string& name) {
    auto it = poorNamesMap.find(name);
    if (it != poorNamesMap.end()) {
        return it->second.suggestedNames;
    }
    return {};
}

void PoorNameChecker::outputErrorMessage(const std::string& name, const int& line) {
    if(poorNamesMap.find(name) != poorNamesMap.end()) {
        const SuggestedNamesInfo& info = poorNamesMap[name];
        std::cout << "Warning: Variable name '" << name << "' is considered poor. Reason: " << info.reason << ". Suggested names: ";
        for (const auto& suggestedName : info.suggestedNames) {
            std::cout << suggestedName << ", ";
        }
    } else if(name.length() < 3 && name != "id") {
        std::cout << "Warning: Variable name '" << name << "' is too short. Consider using a more descriptive name.";

    }

    std::cout << " (line " << line << ")\n\n";
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
                outputErrorMessage(decl.name, actualLine);
                ++warningCount;
            }
        }
    }

    return warningCount;
}
