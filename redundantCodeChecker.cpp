#include "redundantCodeChecker.h"
#include <iostream>
#include <regex>

// Counts whole-word occurrences of `name` within `body` and how many times a function is called/used.
int RedundantCodeChecker::countVariableUsages(const std::string& body, const std::string& name) {
    std::regex wordRegex("\\b" + name + "\\b");
    auto begin = std::sregex_iterator(body.begin(), body.end(), wordRegex);
    auto end = std::sregex_iterator();
    return static_cast<int>(std::distance(begin, end));
}

int RedundantCodeChecker::countFunctionCalls(const std::string& body, const std::string& name) {
    std::regex functionCallRegex("\\b" + name + "\\s*\\(");
    auto begin = std::sregex_iterator(body.begin(), body.end(), functionCallRegex);
    auto end = std::sregex_iterator();
    return static_cast<int>(std::distance(begin, end));
}

int RedundantCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    int warningCount = 0;
    for (const auto& func : parsedSource.functions) {
        for (const auto& variable : func.variables) {
            int variableUsages = countVariableUsages(func.functionBody, variable.name);
            int functionCalls = countFunctionCalls(func.functionBody, variable.name);
            // 1 occurrence = only the declaration itself -> unused
            if (variableUsages <= 1) {
                int actualLine = func.line + variable.line - 1;
                std::cout << "Warning: Redundant dead/unused code. \""
                        << variable.name << "\" is declared but never used. "
                        << "(line " << actualLine << ")\n";
                ++warningCount;
            }
            if (functionCalls <= 1) {
                std::cout << "Warning: Redundant dead/unused code \"" << func.functionStart
                        << "\" is declared but never used. "
                        << "(line " << func.line << ")\n";
                ++warningCount;
            }
        }
    }
    return warningCount;
}