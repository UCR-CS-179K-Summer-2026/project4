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

namespace {

struct DeclaredVariable {
    std::string name;
    int line;
};

// Basic C++ built-in / common type keywords recognized as the start of
// a variable declaration. Extend this list as needed.
const std::vector<std::string> TYPE_KEYWORDS = {
    "int", "float", "double", "char", "bool", "long", "short",
    "unsigned", "auto", "std::string", "string"
};

std::string buildTypePattern() {
    std::ostringstream oss;
    oss << "(?:";
    for (size_t i = 0; i < TYPE_KEYWORDS.size(); ++i) {
        if (i > 0) oss << "|";
        std::string escaped;
        for (char c : TYPE_KEYWORDS[i]) {
            if (c == '.') escaped += "\\.";
            else escaped += c;
        }
        oss << escaped;
    }
    oss << ")";
    return oss.str();
}

// Strips single-line (//) and block (/* */) comments so they don't
// produce false declarations or false "usages".
std::string stripComments(const std::string& source) {
    std::string noBlockComments = std::regex_replace(
        source, std::regex(R"(/\*[\s\S]*?\*/)"), "");
    std::string noLineComments = std::regex_replace(
        noBlockComments, std::regex(R"(//.*)"), "");
    return noLineComments;
}

// Finds variable declarations within a function body (e.g. "int x;",
// "int x = 5;", "int a, b;") and returns each declared name + its
// 1-indexed line number relative to the start of that body.
std::vector<DeclaredVariable> findDeclarations(const std::string& body) {
    std::vector<DeclaredVariable> declarations;

    static const std::string typePattern = buildTypePattern();
    static const std::regex declRegex(
        typePattern + R"(\s+([a-zA-Z_][a-zA-Z0-9_ ,=+*/.()-]*);)"
    );
    static const std::regex declaratorRegex(
        R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:=[^,]*)?)"
    );

    std::istringstream stream(body);
    std::string line;
    int lineNum = 0;

    try {
        while (std::getline(stream, line)) {
            ++lineNum;
            auto begin = std::sregex_iterator(line.begin(), line.end(), declRegex);
            auto end = std::sregex_iterator();

            for (auto it = begin; it != end; ++it) {
                std::string declaratorList = (*it)[1].str();

                auto dBegin = std::sregex_iterator(
                    declaratorList.begin(), declaratorList.end(), declaratorRegex);
                auto dEnd = std::sregex_iterator();

                for (auto dIt = dBegin; dIt != dEnd; ++dIt) {
                    declarations.push_back({(*dIt)[1].str(), lineNum});
                }
            }
        }
    } catch (const std::regex_error& e) {
        std::cerr << "[regex_error in findDeclarations] line " << lineNum
                   << ": \"" << line << "\" code=" << e.code()
                   << " what=" << e.what() << "\n";
        throw;
    }

    return declarations;
}

// Counts whole-word occurrences of `name` within `source`.
int countUsages(const std::string& source, const std::string& name) {
    try {
        std::regex wordRegex("\\b" + name + "\\b");
        auto begin = std::sregex_iterator(source.begin(), source.end(), wordRegex);
        auto end = std::sregex_iterator();
        return static_cast<int>(std::distance(begin, end));
    } catch (const std::regex_error& e) {
        std::cerr << "[regex_error in countUsages] name=\"" << name
                  << "\" code=" << e.code() << " what=" << e.what() << "\n";
        throw;
    }
}

// Finds each top-level function body by matching a signature-like
// pattern ending in "{" and walking forward with brace-depth counting
// to find the matching "}". Keeps each function's variables scoped
// separately. Returns (bodyText, startLineOfBody) pairs.
std::vector<std::pair<std::string, int>> splitIntoFunctionBodies(
    const std::string& source) {

    std::vector<std::pair<std::string, int>> bodies;

    static const std::regex funcStartRegex(
        R"([a-zA-Z_][a-zA-Z0-9_:<>]*\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^;{}]*\)\s*\{)"
    );

    auto begin = std::sregex_iterator(source.begin(), source.end(), funcStartRegex);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        size_t openBracePos = it->position() + it->length() - 1;
        int depth = 1;
        size_t pos = openBracePos + 1;
        size_t bodyStart = pos;

        while (pos < source.size() && depth > 0) {
            if (source[pos] == '{') depth++;
            else if (source[pos] == '}') depth--;
            pos++;
        }

        if (depth == 0) {
            std::string body = source.substr(bodyStart, pos - bodyStart - 1);
            int startLine = static_cast<int>(
                std::count(source.begin(), source.begin() + bodyStart, '\n')) + 1;
            bodies.push_back({body, startLine});
        }
    }

    return bodies;
}

} // namespace

int PoorNameChecker::analyzeFile(std::ifstream& inputFile) {
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string rawSource = buffer.str();
    std::string source = stripComments(rawSource);

    // rewind the stream in case other checkers need to read it too
    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);

    auto functionBodies = splitIntoFunctionBodies(source);

    int warningCount = 0;
    for (const auto& fb : functionBodies) {
        const std::string& body = fb.first;
        int startLine = fb.second;
        std::vector<DeclaredVariable> declarations = findDeclarations(body);

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