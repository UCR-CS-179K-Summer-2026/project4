#include "redundantCodeChecker.h"

#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>

int RedundantCodeChecker::countUsages(const std::string& body, const std::string& variableName) {
    try {
        std::regex wordRegex("\\b" + variableName + "\\b");
        auto begin = std::sregex_iterator(body.begin(), body.end(), wordRegex);
        auto end = std::sregex_iterator();
        return static_cast<int>(std::distance(begin, end));
    } catch (const std::regex_error& e) {
        std::cerr << "[regex_error in countUsages] variableName=\"" << variableName
                  << "\" code=" << e.code() << " what=" << e.what() << "\n";
        throw;
    }
}

int RedundantCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    std::vector<FunctionInfo> functionBodies = parsedSource.functions;

    int warningCount = 0;
    for (const auto& fb : functionBodies) {
        const std::string& body = fb.functionBody;
        int startLine = fb.line;
        std::vector<DeclaredVariable> declarations = fb.variables;

        for (const auto& decl : declarations) {
            int occurrences = countUsages(body, decl.name);
            if (occurrences <= 1) {
                int actualLine = startLine + decl.line - 1;
                std::cout << "Warning: Redundant dead/unused code. \""
                        << decl.name << "\" is declared but never used. "
                        << "(line " << actualLine << ")\n";
                ++warningCount;
            }
        }
    }

    return warningCount;
}