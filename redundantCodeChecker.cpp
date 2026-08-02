#include "redundantCodeChecker.h"
#include <iostream>
#include <regex>

// Counts whole-word occurrences of `name` within `body`.
int RedundantCodeChecker::countUsages(const std::string& body, const std::string& name) {
    std::regex wordRegex("\\b" + name + "\\b");
    auto begin = std::sregex_iterator(body.begin(), body.end(), wordRegex);
    auto end = std::sregex_iterator();
    return static_cast<int>(std::distance(begin, end));
}

int RedundantCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    int warningCount = 0;
 
    for (const auto& func : parsedSource.functions) {
        for (const auto& variable : func.variables) {
            int occurrences = countUsages(func.functionBody, variable.name);
            // 1 occurrence = only the declaration itself -> unused
            if (occurrences <= 1) {
                int actualLine = func.line + variable.line - 1;
                std::cout << "Warning: Redundant dead/unused code. \""
                        << variable.name << "\" is declared but never used. "
                        << "(line " << actualLine << ")\n";
                ++warningCount;
            }
        }
    }
 
    return warningCount;
}