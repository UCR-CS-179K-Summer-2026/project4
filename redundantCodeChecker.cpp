#include "redundantCodeChecker.h"
#include <iostream>
#include <regex>

// Escapes regex metacharacters so identifiers can be safely spliced into a pattern
std::string RedundantCodeChecker::escapeRegex(const std::string& s) {
    static const std::regex specialChars(R"([-[\]{}()*+?.,\^$|#\s])");
    return std::regex_replace(s, specialChars, R"(\$&)");
}

// Counts whole-word occurrences of `name` within `body` and how many times a function is called/used.
int RedundantCodeChecker::countVariableUsages(const std::string& body, const std::string& name) {
    std::regex wordRegex("\\b" + name + "\\b");
    auto begin = std::sregex_iterator(body.begin(), body.end(), wordRegex);
    auto end = std::sregex_iterator();
    return static_cast<int>(std::distance(begin, end));
}

int RedundantCodeChecker::countFunctionCalls(const std::string& source, const std::string& name) {
    std::regex functionCallRegex("\\b" + escapeRegex(name) + "\\s*\\(");
    auto begin = std::sregex_iterator(source.begin(), source.end(), functionCallRegex);
    auto end = std::sregex_iterator();
    return static_cast<int>(std::distance(begin, end));
}

int RedundantCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    int warningCount = 0;
    for (const auto& func : parsedSource.functions) {
        // Search the whole source (not just this function's own body) for calls to it,
        // so external callers are actually detected. A count of 1 means the only match
        // found is the function's own signature — i.e. no real callers.
        int functionCalls = countFunctionCalls(parsedSource.source, func.functionStart);
        if (functionCalls <= 1) {
            std::cout << "Warning: Redundant dead/unused code. The function \"" << func.functionStart
                    << "\" is declared but never used. "
                    << "(line " << func.line << ")\n";
            ++warningCount;
        }
        for (const auto& variable : func.variables) {
            int variableUsages = countVariableUsages(func.functionBody, variable.name);
            // 1 occurrence = only the declaration itself -> unused
            if (variableUsages <= 1) {
                int actualLine = func.line + variable.line - 1;
                std::cout << "Warning: Redundant dead/unused code. The variable \"" << variable.name
                        << "\" is declared but never used. "
                        << "(line " << actualLine << ")\n";
                ++warningCount;
            }
        }
    }
    return warningCount;
}